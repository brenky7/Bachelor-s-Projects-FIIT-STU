package backend;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

/**
 * main.java.backend.ConfigLoader - Načítava konfiguračné hodnoty zo súboru config.properties.
 * Ak súbor neexistuje, alebo sa nepodarí načítať, použijú sa prednastavené hodnoty.
 *
 * @author Peter Brenkus
 */
public class ConfigLoader {
    private static final String CONFIG_FILE = "./config.properties";
    private static final Properties properties = new Properties();

    static {
        try (FileInputStream input = new FileInputStream(CONFIG_FILE)) {
            properties.load(input);
        } catch (IOException e) {
            System.err.println("Chyba pri načítaní konfigurácie: " + e.getMessage());
        }
    }

    public static String get(String key, String defaultValue) {
        return properties.getProperty(key, defaultValue);
    }

    public static int getInt(String key, int defaultValue) {
        try {
            return Integer.parseInt(properties.getProperty(key, String.valueOf(defaultValue)));
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }

    public static boolean getBoolean(String key, boolean defaultValue) {
        return Boolean.parseBoolean(properties.getProperty(key, String.valueOf(defaultValue)));
    }
}
