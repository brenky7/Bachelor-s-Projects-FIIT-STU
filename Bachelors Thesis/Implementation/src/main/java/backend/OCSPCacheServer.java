package backend;

import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.server.ServerConnector;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Base64;
import java.util.Date;
import java.util.List;
import org.eclipse.jetty.util.thread.QueuedThreadPool;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.JedisPoolConfig;
import redis.clients.jedis.exceptions.JedisException;


/**
 * OCSP Cache Server pre cachovanie OCSP odpovedí.
 * Tento server zachytáva OCSP požiadavky, kontroluje cache
 * a v prípade potreby preposiela požiadavku na OCSP responder.
 * Používa:
 * - Jetty ako webový server
 * - Redis na ukladanie odpovedí
 * - HTTP komunikáciu s OCSP responderom
 * - SLF4J pre logovanie
 *
 * @author Peter Brenkus
 */
public class OCSPCacheServer {
    private static final boolean CACHE_ENABLED = ConfigLoader.getBoolean("server.cache.enabled", true);
    private static final int CACHE_TTL = ConfigLoader.getInt("server.cache.ttl", 3600);
    private static final int CACHE_MAX_SIZE = ConfigLoader.getInt("redis.max.size", 10000000);
    private static final int CACHE_SERVER_PORT = ConfigLoader.getInt("server.port", 8080);
    private static final String RESPONDER_URL = ConfigLoader.get("ocsp.responder.url",
            "http://localhost:80/ejbca/publicweb/status/ocsp");
    private static final Logger logger = LoggerFactory.getLogger(OCSPCacheServer.class);
    private static final String REDIS_HOST = ConfigLoader.get("redis.host", "localhost");
    private static final int REDIS_PORT = ConfigLoader.getInt("redis.port", 6379);
    private static final int REDIS_POOL_MAX_TOTAL = ConfigLoader.getInt("redis.threads.max.total", 50);
    private static final int REDIS_POOL_MAX_IDLE = ConfigLoader.getInt("redis.threads.max.idle", 20);
    private static final int REDIS_POOL_MIN_IDLE = ConfigLoader.getInt("redis.threads.min.idle", 5);
    private static final int SERVER_MIN_THREADS = ConfigLoader.getInt("server.threads.min", 20);
    private static final int SERVER_MAX_THREADS = ConfigLoader.getInt("server.threads.max", 500);
    private static final int SERVER_THREAD_TIMEOUT = ConfigLoader.getInt("server.threads.idleTimeout", 60000);
    private static JedisPool jedisPool;

    /**
     * Hlavná metóda, ktorá inicializuje cache server.
     * @throws Exception Ak nastane chyba pri štarte servera
     */
    public static void main(String[] args) throws Exception {
        logger.info("Spúšťam OCSP Cache Server...");
        if (CACHE_ENABLED) {
            JedisPoolConfig poolConfig = new JedisPoolConfig();
            poolConfig.setTestOnBorrow(true);
            poolConfig.setMaxTotal(REDIS_POOL_MAX_TOTAL);
            poolConfig.setMaxIdle(REDIS_POOL_MAX_IDLE);
            poolConfig.setMinIdle(REDIS_POOL_MIN_IDLE);

            logger.info("Konfigurujem JedisPool pre Redis na {}:{} s MaxTotal={}, MaxIdle={}, MinIdle={}",
                    REDIS_HOST, REDIS_PORT, REDIS_POOL_MAX_TOTAL, REDIS_POOL_MAX_IDLE, REDIS_POOL_MIN_IDLE);
            jedisPool = new JedisPool(poolConfig, REDIS_HOST, REDIS_PORT);

            try (Jedis jedis = jedisPool.getResource()) {
                String pingResponse = jedis.ping();
                if (!"PONG".equalsIgnoreCase(pingResponse)) {
                    logger.error("Nepodarilo sa pingnúť Redis server. Odpoveď: {}. Vypínam server.", pingResponse);
                    jedisPool.close();
                    return;
                }
                jedis.configSet("maxmemory", String.valueOf(CACHE_MAX_SIZE));
                logger.info("Nastavená maximálna veľkosť cache na: {} bajtov", CACHE_MAX_SIZE);
            } catch (JedisException e) {
                logger.error("Kritická chyba pri pripájaní k Redis alebo nastavovaní konfigurácie cez JedisPool. Vypínam server.", e);
                if (jedisPool != null && !jedisPool.isClosed()) {
                    jedisPool.close();
                }
                throw e;
            }
        }

        logger.info("OCSP responder URL: {}", RESPONDER_URL);
        logger.info("Cache je " + (CACHE_ENABLED ? "zapnutá" : "vypnutá"));
        if (CACHE_ENABLED) {
            logger.info("Predvolený Cache TTL: " + CACHE_TTL + " sekúnd");
        }

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            logger.info("Prijatý signál na ukončenie.");
            if (CACHE_ENABLED && jedisPool != null && !jedisPool.isClosed()) {
                jedisPool.close();
                logger.info("JedisPool úspešne uzavretý.");
            }
            logger.info("Vypínam server.");
        }));

        startServer();
    }

    /**
     * Inicializuje a spustí webový server.
     * @throws Exception Ak nastane chyba pri spustení servera
     */
    private static void startServer() throws Exception {
        QueuedThreadPool threadPool = new QueuedThreadPool();
        threadPool.setMinThreads(SERVER_MIN_THREADS);
        threadPool.setMaxThreads(SERVER_MAX_THREADS);
        threadPool.setIdleTimeout(SERVER_THREAD_TIMEOUT);

        Server server = new Server(threadPool);
        ServerConnector connector = new ServerConnector(server);
        connector.setPort(CACHE_SERVER_PORT);
        server.addConnector(connector);

        ServletContextHandler handler = new ServletContextHandler(ServletContextHandler.SESSIONS);
        handler.setContextPath("/");
        server.setHandler(handler);

        handler.addServlet(new ServletHolder(new OCSPCacheServlet()), "/ocsp/*");

        server.start();
        logger.info("OCSP Cache Server úspešne spustený na porte {}", CACHE_SERVER_PORT);
        server.join();
    }

    /**
     * Servlet pre spracovanie OCSP požiadaviek.
     */
    public static class OCSPCacheServlet extends HttpServlet {
        private static final Logger logger = LoggerFactory.getLogger(OCSPCacheServlet.class);

        /**
         * Spracováva prichádzajúce POST požiadavky s OCSP requestami.
         * @param request  HTTP požiadavka obsahujúca OCSP request
         * @param response HTTP odpoveď s OCSP odpoveďou
         * @throws IOException Ak nastane chyba pri čítaní/zápise dát
         */
        protected void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
            byte[] requestBody = request.getInputStream().readAllBytes();
            logger.info("Prijatá POST OCSP požiadavka veľkosti: {} bajtov", requestBody.length);
            processOcspRequest(requestBody, response);
        }

        /**
         * Spracováva prichádzajúce GET požiadavky s OCSP requestami.
         * @param request  HTTP požiadavka s OCSP requestom v URL
         * @param response HTTP odpoveď s OCSP odpoveďou
         * @throws IOException Ak nastane chyba pri čítaní/zápise dát
         */
        protected void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
            // Načítanie OCSP požiadavky z URL cesty
            String requestKey = request.getPathInfo();

            // Kontrola, či požiadavka obsahuje nejaké dáta
            if (requestKey == null || requestKey.length() <= 1) {
                logger.warn("Chýbajúci OCSP request v GET URL");
                response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Chýbajúci OCSP request v URL");
                return;
            }

            // Dekódovanie OCSP requestu z URL
            try {
                byte[] ocspRequestData = Base64.getUrlDecoder().decode(requestKey.substring(1));
                logger.info("Prijatá GET OCSP požiadavka, dekódovaná veľkosť: {} bajtov", ocspRequestData.length);
                processOcspRequest(ocspRequestData, response);
            } catch (IllegalArgumentException e) {
                // Chyba pri dekódovaní OCSP requestu
                logger.error("Neplatný OCSP request encoding");
                response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Neplatný OCSP request encoding");
            }
        }

        /**
         * Spracuje OCSP požiadavku – načíta z cache alebo odošle na OCSP responder.
         * @param ocspRequestData Dáta OCSP požiadavky
         * @param response        HTTP odpoveď obsahujúca OCSP odpoveď
         * @throws IOException Ak nastane chyba pri spracovaní requestu
         */
        private void processOcspRequest(byte[] ocspRequestData, HttpServletResponse response) throws IOException {
            // Kontrola, či OCSP požiadavka obsahuje nonce
            boolean hasNonce = OCSPRequestParser.hasNonce(ocspRequestData);
            if (hasNonce) {
                logger.debug("OCSP požiadavka obsahuje nonce, presmerujem ju na responder bez cachovania.");
                byte[] ocspResponseBytes = fetchFromResponder(ocspRequestData);
                try{
                    OCSPResponseParser.parseOCSPResponse(ocspResponseBytes);
                } catch (Exception e) {
                    logger.error("Chyba pri parsovaní OCSP odpovede");
                    response.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Chyba pri parsovaní OCSP odpovede");
                    return;
                }

                if (CACHE_ENABLED) {
                    response.setHeader("OCSP-Cache", "BYPASS_NONCE");
                }
                else {
                    response.setHeader("OCSP-Cache", "DISABLED");
                }
                response.setContentType("application/ocsp-response");
                response.setContentLength(ocspResponseBytes.length);
                response.getOutputStream().write(ocspResponseBytes);
                return;
            }

            // Parsovanie OCSP požiadavky a extrakcia sériových čísel certifikátov
            List<String> serialNumbers;
            try {
                serialNumbers = OCSPRequestParser.parseOCSPRequest(ocspRequestData);
            } catch (IOException e) {
                logger.error("Chyba pri parsovaní OCSP requestu");
                response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Chyba pri parsovaní OCSP requestu");
                return;
            }

            // Vytvorenie unikátneho kľúča pre cache na základe sériových čísel
            String requestKey = String.join(",", serialNumbers);
            byte[] ocspResponseBytes = null;
            boolean cacheHit = false;

            // Pokus načítať OCSP odpoveď z cache, ak je cache zapnutá
            if (CACHE_ENABLED) {
                try (Jedis jedis = jedisPool.getResource()) {
                    String cachedResponseBase64 = jedis.get(requestKey);
                    if (cachedResponseBase64 != null) {
                        ocspResponseBytes = Base64.getDecoder().decode(cachedResponseBase64);
                        logger.info("OCSP odpoveď načítaná z cache pre kľúč: {}", requestKey);
                        cacheHit = true;
                    }
                } catch (JedisException e) {
                    logger.error("Chyba pri čítaní z Redis cache pre kľúč '{}': {}", requestKey, e.getMessage(), e);
                }
            }

            // Ak odpoveď nebola nájdená v cache, získa sa z OCSP respondera
            if (!cacheHit) {
                ocspResponseBytes = fetchFromResponder(ocspRequestData);
            }

            // Určenie TTL podľa nextUpdate ak je nastavený, inak default
            int ttlToUse = CACHE_TTL;
            if (CACHE_ENABLED) {
                try {
                    OCSPResponseParser.parseOCSPResponse(ocspResponseBytes);
                    Date nextUpdate = OCSPResponseParser.getNextUpdate(ocspResponseBytes);
                    if (nextUpdate != null && !cacheHit) {
                        long ttlMillis = nextUpdate.getTime() - System.currentTimeMillis();
                        int ttlSeconds = (int) (ttlMillis / 1000);
                        if (ttlSeconds > 0) {
                            ttlToUse = ttlSeconds;
                            logger.debug("Používam TTL z nextUpdate: {} sekúnd", ttlToUse);
                        }
                    }
                } catch (Exception e) {
                    logger.debug("Nepodarilo sa extrahovať informácie z OCSP odpovede, používa sa predvolený TTL: {} sekúnd", CACHE_TTL);
                }
            }

            // Uloženie odpovede do cache ak je cache zapnutá a nebola tam predtým
            if (CACHE_ENABLED && !cacheHit) {
                try (Jedis jedis = jedisPool.getResource()) {
                    jedis.setex(requestKey, ttlToUse, Base64.getEncoder().encodeToString(ocspResponseBytes));
                    logger.info("OCSP odpoveď uložená do cache s kľúčom: '{}' a TTL: {} sekúnd", requestKey, ttlToUse);
                } catch (JedisException e) {
                    logger.error("Chyba pri zápise do Redis cache pre kľúč '{}': {}", requestKey, e.getMessage(), e);
                }
            }

            if (!CACHE_ENABLED) {
                response.setHeader("OCSP-Cache", "DISABLED");
            } else {
                response.setHeader("OCSP-Cache", cacheHit ? "HIT" : "MISS");
            }

            // Odoslanie OCSP odpovede klientovi
            response.setContentType("application/ocsp-response");
            response.setContentLength(ocspResponseBytes.length);
            response.getOutputStream().write(ocspResponseBytes);
        }

        /**
         * Odošle OCSP požiadavku na OCSP responder a vráti odpoveď.
         * @param ocspRequestData Dáta OCSP požiadavky
         * @return OCSP odpoveď ako pole bajtov
         * @throws IOException Ak nastane chyba pri komunikácii s OCSP responderom
         */
        private byte[] fetchFromResponder(byte[] ocspRequestData) throws IOException {
            URL url = new URL(RESPONDER_URL);
            HttpURLConnection con = null;
            try {
                // Konfigurácia HTTP spojenia
                con = (HttpURLConnection) url.openConnection();
                con.setRequestMethod("POST");
                con.setDoOutput(true);
                con.setConnectTimeout(5000);
                con.setReadTimeout(10000);
                con.setRequestProperty("Content-Type", "application/ocsp-request");
                con.setRequestProperty("Accept", "application/ocsp-response");

                // Odoslanie OCSP požiadavky na server
                try (OutputStream os = con.getOutputStream()) {
                    os.write(ocspRequestData);
                }

                // Získanie HTTP status kódu odpovede
                int responseCode = con.getResponseCode();
                logger.debug("OCSP Responder Response Code: {}", responseCode);

                // Načítanie OCSP odpovede
                byte[] ocspResponseBytes;
                try (InputStream inputStream = (responseCode >= 200 && responseCode < 300) ?
                        con.getInputStream() : con.getErrorStream()) {

                    if (inputStream != null) {
                        ocspResponseBytes = inputStream.readAllBytes();
                    } else {
                        logger.warn("OCSP responder vrátil kód {} bez response streamu.", responseCode);
                        ocspResponseBytes = new byte[0];
                    }
                }

                if (responseCode < 200 || responseCode >= 300) {
                    logger.error("OCSP responder error {}. Odpoveď (ak bola): {}", responseCode, new String(ocspResponseBytes));
                }

                return ocspResponseBytes;

            // Zatvorenie Http spojenia
            } finally {
                if (con != null) {
                    con.disconnect();
                    logger.trace("HttpURLConnection disconnected.");
                }
            }
        }
    }
}
