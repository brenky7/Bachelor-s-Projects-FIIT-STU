FROM eclipse-temurin:17-jre-focal
WORKDIR /app
COPY target/ocsp-cache-server-1.0.jar /app/app.jar
COPY config.properties /app/config.properties
COPY logback.xml /app/logback.xml
COPY test_remote/ca.pem /app/ca.pem
COPY test_remote/request1.der /app/request1.der
ENTRYPOINT ["java", "-Dlogback.configurationFile=/app/logback.xml", "-jar", "/app/app.jar"]
