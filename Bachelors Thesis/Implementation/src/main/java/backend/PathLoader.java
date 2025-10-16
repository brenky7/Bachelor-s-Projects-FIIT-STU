package backend;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.Base64;

/**
 * main.java.backend.PathLoader - Načítava kryptografické kľúče a certifikáty zo súborov.
 * Táto trieda umožňuje:
 * - Načítať privátny kľúč z PEM súboru
 * - Načítať verejný kľúč z PEM súboru
 * - Načítať X.509 certifikát zo súboru
 *
 * @author Peter Brenkus
 */
public class PathLoader {
    private static final Logger logger = LoggerFactory.getLogger(PathLoader.class);

    /**
     * Načíta privátny kľúč z PEM súboru.
     * @param keyPath Cesta k PEM súboru s privátnym kľúčom
     * @return PrivateKey objekt
     */
    public static PrivateKey loadPrivateKey(String keyPath) throws Exception {
        String keyContent = readKeyFile(keyPath);

        // Odstránenie hlavičky a päty PEM formátu
        keyContent = keyContent.replace("-----BEGIN PRIVATE KEY-----", "")
                .replace("-----END PRIVATE KEY-----", "")
                .replaceAll("\\s", "");

        // Dekódovanie Base64 obsahu
        byte[] keyBytes = Base64.getDecoder().decode(keyContent);
        PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");

        return keyFactory.generatePrivate(keySpec);
    }

    /**
     * Načíta verejný kľúč z PEM súboru a vráti objekt {@link PublicKey}.
     * @param keyPath Cesta k PEM súboru obsahujúcemu verejný kľúč.
     * @return PublicKey objekt reprezentujúci načítaný kľúč.
     * @throws Exception Ak nastane chyba pri načítaní alebo spracovaní kľúča.
     *
     * Nakoniec sa nepoužíva, ale je tu pre prípadné rozšírenie.
     */
    public static PublicKey loadPublicKey(String keyPath) throws Exception {
        String keyContent = readKeyFile(keyPath);

        // Odstránenie hlavičky a päty PEM formátu
        keyContent = keyContent.replace("-----BEGIN PUBLIC KEY-----", "")
                .replace("-----END PUBLIC KEY-----", "")
                .replaceAll("\\s", "");

        // Dekódovanie Base64 obsahu
        byte[] keyBytes = Base64.getDecoder().decode(keyContent);
        X509EncodedKeySpec keySpec = new X509EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");

        return keyFactory.generatePublic(keySpec);
    }

    /**
     * Načíta X.509 certifikát zo súboru a vráti objekt {@link X509Certificate}.
     * @param certPath Cesta k súboru obsahujúcemu X.509 certifikát.
     * @return X509Certificate objekt reprezentujúci načítaný certifikát.
     * @throws Exception Ak nastane chyba pri načítaní alebo spracovaní certifikátu.
     */
    public static X509Certificate loadCertificate(String certPath) throws Exception {
        try (FileInputStream fis = new FileInputStream(certPath)) {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            return (X509Certificate) cf.generateCertificate(fis);
        } catch (Exception e) {
            logger.error("Chyba pri načítaní certifikátu zo súboru: {}", certPath, e);
            throw e;
        }
    }

    /**
     * Pomocná metóda na čítanie obsahu PEM súboru do String formátu.
     * @param path Cesta k súboru s kľúčom alebo certifikátom.
     * @return Reťazec obsahujúci Base64 reprezentáciu kľúča alebo certifikátu.
     * @throws IOException Ak nastane chyba pri čítaní súboru.
     */
    private static String readKeyFile(String path) throws IOException {
        StringBuilder keyContent = new StringBuilder();
        try (BufferedReader reader = new BufferedReader(new FileReader(path, StandardCharsets.UTF_8))) {
            String line;
            while ((line = reader.readLine()) != null) {
                keyContent.append(line).append("\n");
            }
        }
        return keyContent.toString();
    }
}

