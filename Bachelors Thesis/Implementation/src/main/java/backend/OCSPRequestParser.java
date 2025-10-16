package backend;

import org.bouncycastle.asn1.ASN1InputStream;
import org.bouncycastle.asn1.ASN1Primitive;
import org.bouncycastle.asn1.ocsp.OCSPObjectIdentifiers;
import org.bouncycastle.cert.ocsp.OCSPReq;
import org.bouncycastle.cert.ocsp.Req;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * main.java.backend.OCSPRequestParser - Dekóduje OCSP požiadavky a extrahuje sériové čísla certifikátov.
 * Táto trieda umožňuje:
 * - Načítať a dekódovať OCSP požiadavku vo formáte ASN.1.
 * - Extrahovať sériové čísla certifikátov zahrnutých v požiadavke.
 * - Vrátiť zoznam sériových čísel v hexadecimálnom formáte.
 * Používa:
 * - BouncyCastle na prácu s OCSP požiadavkami.
 *
 * @author Peter Brenkus
 */
public class OCSPRequestParser {

    /**
     * Parsuje OCSP požiadavku a extrahuje sériové čísla certifikátov.
     * @param ocspRequestData Byte array obsahujúci OCSP požiadavku v ASN.1 formáte.
     * @return Zoznam sériových čísel certifikátov vo formáte HEX (uppercase).
     * @throws IOException Ak nastane chyba pri dekódovaní OCSP požiadavky.
     */
    public static List<String> parseOCSPRequest(byte[] ocspRequestData) throws IOException {
        List<String> serialNumbers = new ArrayList<>();

        // Vytvorenie ASN.1 input streamu na dekódovanie OCSP požiadavky
        try (ASN1InputStream asn1InputStream = new ASN1InputStream(new ByteArrayInputStream(ocspRequestData))) {
            ASN1Primitive asn1Primitive = asn1InputStream.readObject();

            // Dekódovanie OCSP požiadavky z ASN.1 objektu
            OCSPReq ocspReq = new OCSPReq(asn1Primitive.getEncoded());

            // Spracovanie všetkých požiadaviek v OCSPRequest
            for (Req req : ocspReq.getRequestList()) {
                // Extrakcia sériového čísla certifikátu v HEX formáte
                String serialNumber = req.getCertID().getSerialNumber().toString(16).toUpperCase();
                serialNumbers.add(serialNumber);
            }
        }

        return serialNumbers;
    }

    public static boolean hasNonce(byte[] ocspRequestData) {
        try (ASN1InputStream asn1InputStream = new ASN1InputStream(new ByteArrayInputStream(ocspRequestData))) {
            ASN1Primitive asn1Primitive = asn1InputStream.readObject();
            OCSPReq ocspReq = new OCSPReq(asn1Primitive.getEncoded());
            return ocspReq.getExtension(OCSPObjectIdentifiers.id_pkix_ocsp_nonce) != null;
        } catch (IOException e) {
            return false;
        }
    }
}

