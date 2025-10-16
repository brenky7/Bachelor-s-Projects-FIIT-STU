package test; /**
 * test.OCSPResponder - jednoduchá náhrada za reálnu implementáciu.
 * Využitie pre testovanie a vývoj.
 *
 * Tento OCSP responder:
 * - Prijíma OCSP požiadavky cez HTTP POST.
 * - Extrahuje sériové číslo certifikátu z OCSP požiadavky.
 * - Generuje OCSP odpoveď s použitím {@link test.OCSPResponseGenerator}.
 * - Posiela OCSP odpoveď späť klientovi.
 *
 * Používa:
 * - Jetty na HTTP server.
 * - SLF4J na logovanie.
 * - {@link main.java.backend.OCSPRequestParser} na dekódovanie OCSP požiadaviek.
 * - {@link test.OCSPResponseGenerator} na generovanie odpovedí.
 *
 * @author Peter Brenkus
 */
import main.java.backend.ConfigLoader;
import main.java.backend.OCSPRequestParser;
import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.math.BigInteger;
import java.util.List;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class OCSPResponder {
    private static final int RESPONDER_PORT = ConfigLoader.getInt("r.port", 9090);
    private static final Logger logger = LoggerFactory.getLogger(OCSPResponder.class);

    /**
     * Hlavná metóda spúšťajúca OCSP responder.
     * @param args Argumenty príkazového riadku (nepoužívajú sa).
     * @throws Exception Ak nastane chyba pri štarte servera.
     */
    public static void main(String[] args) throws Exception {
        logger.info("Spúšťam OCSP Responder na porte " + RESPONDER_PORT + "...");
        startServer();
    }

    /**
     * Inicializuje a spustí HTTP server.
     * @throws Exception Ak nastane chyba pri spustení servera.
     */
    private static void startServer() throws Exception {
        Server server = new Server(RESPONDER_PORT);
        ServletContextHandler handler = new ServletContextHandler(ServletContextHandler.SESSIONS);
        handler.setContextPath("/");
        server.setHandler(handler);

        // Pridanie servletu pre spracovanie OCSP požiadaviek
        handler.addServlet(new ServletHolder(new OCSPResponderServlet()), "/ocsp/*");

        server.start();
        server.join();
    }

    /**
     * Servlet na spracovanie OCSP požiadaviek.
     */
    public static class OCSPResponderServlet extends HttpServlet {
        private static final Logger logger = LoggerFactory.getLogger(OCSPResponderServlet.class);

        /**
         * Spracuje OCSP požiadavku zaslanú cez HTTP POST.
         * @param request  HTTP požiadavka obsahujúca OCSP request.
         * @param response HTTP odpoveď s OCSP odpoveďou.
         * @throws IOException Ak nastane chyba pri spracovaní požiadavky.
         */
        protected void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
            try {
                // Načítanie obsahu OCSP požiadavky
                byte[] requestBody = request.getInputStream().readAllBytes();
                logger.info("Prijatá POST OCSP požiadavka veľkosti: {} bajtov", requestBody.length);

                // Parsovanie OCSP requestu na získanie sériových čísel certifikátov
                List<String> serialNumbers = OCSPRequestParser.parseOCSPRequest(requestBody);

                // Ak neboli nájdené žiadne sériové čísla, odpovedať s chybou
                if (serialNumbers.isEmpty()) {
                    response.sendError(HttpServletResponse.SC_BAD_REQUEST, "No serial numbers found in request.");
                    return;
                }

                // Konverzia prvého sériového čísla na BigInteger
                BigInteger serialNumber = new BigInteger(serialNumbers.get(0), 16);

                // Generovanie OCSP odpovede
                OCSPResponseGenerator ocspResponseGenerator = new OCSPResponseGenerator();
                byte[] ocspResponse = ocspResponseGenerator.generateOCSPResponse(serialNumber);

                // Odoslanie OCSP odpovede
                response.setContentType("application/ocsp-response");
                response.getOutputStream().write(ocspResponse);
                logger.info("OCSP odpoveď úspešne odoslaná.");
            } catch (Exception e) {
                // Ak nastane chyba, odpovedať s HTTP 500 a zalogovať chybu
                response.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error generating OCSP response.");
                logger.error("Error generating OCSP response", e);
            }
        }
    }
}

