package backend;

import org.bouncycastle.cert.ocsp.BasicOCSPResp;
import org.bouncycastle.cert.ocsp.CertificateID;
import org.bouncycastle.cert.ocsp.OCSPResp;
import org.bouncycastle.cert.ocsp.SingleResp;
import org.slf4j.LoggerFactory;
import org.slf4j.Logger;
import java.security.Signature;
import java.security.PublicKey;
import java.security.cert.X509Certificate;
import java.util.Date;

/**
 * main.java.backend.OCSPResponseParser - Analyzuje OCSP odpoveď a overuje jej podpis.
 * Používa:
 * - BouncyCastle pre prácu s OCSP odpoveďami
 * - SLF4J na logovanie
 * - Java Cryptography API na overenie podpisu
 *
 * @author Peter Brenkus
 */
public class OCSPResponseParser {

    private static final Logger logger = LoggerFactory.getLogger(OCSPResponseParser.class);

    /**
     * Spracuje OCSP odpoveď, vypíše jej obsah a overí podpis.
     * @param ocspResponseBytes Byte array obsahujúci OCSP odpoveď.
     */
    public static void parseOCSPResponse(byte[] ocspResponseBytes) {
        try {
            // Dekódovanie základnej OCSP odpovede
            OCSPResp ocspResp = new OCSPResp(ocspResponseBytes);
            int status = ocspResp.getStatus();

            // Ak OCSP odpoveď nie je úspešná, ukončí sa spracovanie
            if (status != OCSPResp.SUCCESSFUL) {
                logger.debug("Status odpovede: " + getResponseStatusText(status));
                logger.debug("OCSP odpoveď nebola úspešná, končím spracovanie.");
                return;
            }

            // Spracovanie jednotlivých odpovedí pre certifikáty
            BasicOCSPResp basicResponse = (BasicOCSPResp) ocspResp.getResponseObject();
            SingleResp[] responses = basicResponse.getResponses();
            for (SingleResp singleResp : responses) {
                CertificateID certID = singleResp.getCertID();
                Object certStatus = singleResp.getCertStatus();
                Date thisUpdate = singleResp.getThisUpdate();
                Date nextUpdate = singleResp.getNextUpdate();

                logger.debug("Certifikát Serial Number: {}", certID.getSerialNumber().toString(16).toUpperCase());
                logger.debug("Stav certifikátu: {}", getCertStatusText(certStatus));
                logger.debug("Dátum kontroly: {}", thisUpdate);
                if (nextUpdate != null) {
                    logger.debug("Ďalšia aktualizácia: {}", nextUpdate);
                }
            }

            // Načítanie OCSP responder certifikátu na overenie podpisu
            X509Certificate responderCert = PathLoader.loadCertificate(ConfigLoader.get("ca.cert", "test/ejbca/ca.pem"));
            boolean isValid = verifyOCSPSignature(basicResponse, responderCert.getPublicKey());
            logger.debug("Platnosť podpisu OCSP odpovede: {}", isValid ? "Platný" : "Neplatný");
        } catch (Exception e) {
            logger.debug("Chyba pri dekódovaní OCSP odpovede.");
        }
    }

    /**
     * Overí podpis OCSP odpovede pomocou verejného kľúča respondera.
     * @param response  OCSP odpoveď (BasicOCSPResp) obsahujúca podpis.
     * @param publicKey Verejný kľúč OCSP respondera.
     * @return true, ak je podpis platný, inak false.
     * @throws Exception Ak nastane chyba pri overovaní podpisu.
     */
    private static boolean verifyOCSPSignature(BasicOCSPResp response, PublicKey publicKey) throws Exception {
        Signature signature = Signature.getInstance("SHA256withRSA");
        signature.initVerify(publicKey);
        byte[] tbsResponseData = response.getTBSResponseData();

        signature.update(tbsResponseData);
        return signature.verify(response.getSignature());
    }

    /**
     * Konvertuje OCSP status na čitateľný textový reťazec.
     * @param status OCSP status kód.
     * @return Textový reťazec reprezentujúci OCSP status.
     */
    private static String getResponseStatusText(int status) {
        return switch (status) {
            case OCSPResp.SUCCESSFUL -> "SUCCESSFUL";
            case OCSPResp.MALFORMED_REQUEST -> "MALFORMED_REQUEST";
            case OCSPResp.INTERNAL_ERROR -> "INTERNAL_ERROR";
            case OCSPResp.TRY_LATER -> "TRY_LATER";
            case OCSPResp.SIG_REQUIRED -> "SIG_REQUIRED";
            case OCSPResp.UNAUTHORIZED -> "UNAUTHORIZED";
            default -> "UNKNOWN";
        };
    }

    /**
     * Získa textovú reprezentáciu stavu certifikátu v OCSP odpovedi.
     * @param certStatus Objekt reprezentujúci stav certifikátu.
     * @return Reťazec "GOOD", "REVOKED" alebo "UNKNOWN" podľa stavu certifikátu.
     */
    private static String getCertStatusText(Object certStatus) {
        // Predvolený stav certStatus je null, vtedy je certifikát "GOOD"
        if (certStatus == null) {
            return "GOOD";
        } else if (certStatus instanceof org.bouncycastle.cert.ocsp.RevokedStatus) {
            return "REVOKED";
        } else {
            return "UNKNOWN";
        }
    }

    public static Date getNextUpdate(byte[] ocspResponseBytes) throws Exception {
        OCSPResp ocspResp = new OCSPResp(ocspResponseBytes);
        BasicOCSPResp basicResponse = (BasicOCSPResp) ocspResp.getResponseObject();
        return basicResponse.getResponses()[0].getNextUpdate();
    }
}
