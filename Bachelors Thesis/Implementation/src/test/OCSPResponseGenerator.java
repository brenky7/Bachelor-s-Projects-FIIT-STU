package test; /**
 * Pomocná trieda pre generovanie OCSP odpovedí.
 * Generuje OCSP odpovede pre dané sériové číslo certifikátu.
 *
 * Táto trieda umožňuje:
 * - Vygenerovať OCSP odpoveď pre konkrétny certifikát.
 * - Podpísať OCSP odpoveď pomocou privátneho kľúča respondera.
 *
 * Používa:
 * - BouncyCastle na prácu s OCSP odpoveďami a kryptografickými operáciami.
 * - SLF4J na logovanie.
 *
 * @author Peter Brenkus
 */
import main.java.backend.ConfigLoader;
import main.java.backend.PathLoader;
import org.bouncycastle.asn1.nist.NISTObjectIdentifiers;
import org.bouncycastle.asn1.pkcs.PKCSObjectIdentifiers;
import org.bouncycastle.asn1.x509.AlgorithmIdentifier;
import org.bouncycastle.cert.ocsp.*;
import org.bouncycastle.cert.ocsp.jcajce.JcaBasicOCSPRespBuilder;
import org.bouncycastle.operator.ContentSigner;
import org.bouncycastle.operator.DigestCalculator;
import org.bouncycastle.operator.bc.BcDigestCalculatorProvider;
import org.bouncycastle.cert.X509CertificateHolder;
import org.bouncycastle.operator.bc.BcRSAContentSignerBuilder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.bouncycastle.crypto.params.AsymmetricKeyParameter;
import org.bouncycastle.crypto.util.PrivateKeyFactory;
import java.math.BigInteger;
import java.security.PrivateKey;
import java.security.cert.X509Certificate;
import java.util.Date;

public class OCSPResponseGenerator {
    private final PrivateKey responderKey;
    private final X509Certificate responderCert;
    private static final Logger logger = LoggerFactory.getLogger(OCSPResponseGenerator.class);

    /**
     * Inicializuje triedu a načíta privátny kľúč a certifikát.
     * @throws Exception Ak nastane chyba pri načítaní kľúča alebo certifikátu.
     */
    public OCSPResponseGenerator() throws Exception {
        String privateKeyPath = ConfigLoader.get("private.key", "test/keys/responder-key.pem");
        String certPath = ConfigLoader.get("cert", "test/keys/responder-cert.pem");

        this.responderKey = PathLoader.loadPrivateKey(privateKeyPath);
        this.responderCert = PathLoader.loadCertificate(certPath);
    }

    /**
     * Generuje OCSP odpoveď pre zadané sériové číslo certifikátu.
     * @param serialNumber Sériové číslo certifikátu, ktorý sa overuje.
     * @return Byte array obsahujúci OCSP odpoveď.
     * @throws Exception Ak nastane chyba pri generovaní OCSP odpovede.
     */
    public byte[] generateOCSPResponse(BigInteger serialNumber) throws Exception {
        DigestCalculator digestCalculator = new BcDigestCalculatorProvider().get(CertificateID.HASH_SHA1);

        // Vytvorenie OCSP request ID (overovaný certifikát)
        X509CertificateHolder responderCertHolder = new X509CertificateHolder(responderCert.getEncoded());
        CertificateID certId = new CertificateID(digestCalculator, responderCertHolder, serialNumber);

        // Vytvorenie OCSP odpovede
        BasicOCSPRespBuilder basicRespBuilder = new JcaBasicOCSPRespBuilder(
                responderCert.getPublicKey(), digestCalculator);

        // Pridanie odpovede s GOOD statusom, bez logiky pre reálne overovanie
        basicRespBuilder.addResponse(certId, CertificateStatus.GOOD);

        // Konverzia privátneho kľúča do BouncyCastle formátu
        AsymmetricKeyParameter privateKeyParam = PrivateKeyFactory.createKey(responderKey.getEncoded());

        // Získanie identifikátorov algoritmov
        AlgorithmIdentifier sigAlgId = new AlgorithmIdentifier(PKCSObjectIdentifiers.sha256WithRSAEncryption);
        AlgorithmIdentifier digAlgId = new AlgorithmIdentifier(NISTObjectIdentifiers.id_sha256);

        // Vytvorenie ContentSignera na podpis OCSP odpovede
        ContentSigner signer = new BcRSAContentSignerBuilder(sigAlgId, digAlgId).build(privateKeyParam);
        BasicOCSPResp basicResp = basicRespBuilder.build(signer, null, new Date());

        // Zostavenie OCSP odpovede
        OCSPRespBuilder respBuilder = new OCSPRespBuilder();
        OCSPResp ocspResp = respBuilder.build(OCSPRespBuilder.SUCCESSFUL, basicResp);
        logger.info("OCSP odpoveď úspešne vygenerovaná.");
        return ocspResp.getEncoded();
    }
}
