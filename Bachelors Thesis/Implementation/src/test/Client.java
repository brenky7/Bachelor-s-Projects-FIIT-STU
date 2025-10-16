package test; /**
 * test.Client - Simuluje viacero klientov odosielajúcich OCSP požiadavky na server.
 * Nedokončený kód, nepoužíva sa.
 *
 * @author Peter Brenkus
 */
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Client {
    private static final int THREADS = 10;
    private static final int REQUESTS_PER_CLIENT = 100;
    private static final String SERVER_URL = "http://localhost:8080/ocsp";

    public static void main(String[] args) {
        ExecutorService executor = Executors.newFixedThreadPool(THREADS);

        for (int i = 0; i < THREADS; i++) {
            executor.submit(() -> {
                for (int j = 0; j < REQUESTS_PER_CLIENT; j++) {
                    sendPostRequest("cert-" + (j % 5));
                }
            });
        }

        executor.shutdown();
    }

    private static void sendPostRequest(String certId) {
        try {
            URL url = new URL(SERVER_URL);
            HttpURLConnection con = (HttpURLConnection) url.openConnection();
            con.setRequestMethod("POST");
            con.setDoOutput(true);

            try (OutputStream os = con.getOutputStream()) {
                os.write(certId.getBytes());
            }

            int responseCode = con.getResponseCode();
            System.out.println("Cert " + certId + " - Response Code: " + responseCode);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

