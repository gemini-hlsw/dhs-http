package edu.gemini.dhs.translator.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.HttpClientBuilder;
import org.junit.Test;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class DhsTranslatorTest {
    private static final Integer numberOfThreads = 100;
    private final static String BASE_URI = "http://localhost:9090/axis2/services/dhs/images";
    private static boolean isNotOk = false;

    @Test
    public void testCreateImage() throws ClientProtocolException, IOException {
        HttpClient client = HttpClientBuilder.create().build();
        HttpPost post = new HttpPost(BASE_URI);

        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        reqNode.with("createImage");

        StringEntity input = new StringEntity(reqNode.toString());
        post.setEntity(input);
        post.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(post);
        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");
        assertNotNull("Respose did not have response node.", responseNode);

        JsonNode status = responseNode.get("status");
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation failed.", "success", status.asText());

        JsonNode result = responseNode.get("result");
        assertNotNull("Respose did not contain image id.", result);
        assertFalse("Returned image id was empty.", result.asText().isEmpty());
    }

    @Test
    public void testCreateImageWithParams() throws ClientProtocolException,
            IOException {
        HttpClient client = HttpClientBuilder.create().build();

        HttpPost post = new HttpPost(BASE_URI);

        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        reqNode.with("createImage").put("lifetime", "PERMANENT")
                .putArray("contributors").add("dhs-http").add("gsaoi");

        StringEntity input = new StringEntity(reqNode.toString());
        post.setEntity(input);
        post.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(post);
        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");

        assertNotNull("Respose did not have response node.", responseNode);

        JsonNode status = responseNode.get("status");
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation failed.", "success", status.asText());

        JsonNode result = responseNode.get("result");
        assertNotNull("Respose did not contained image id.", result);
        assertFalse("Returned image id was empty.", result.asText().isEmpty());
    }

    @Test
    public void testSetParams() throws ClientProtocolException, IOException {
        HttpClient client = HttpClientBuilder.create().build();
        String imageId = getImage();

        HttpPut put = new HttpPut(BASE_URI + "/" + imageId);
        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        reqNode.with("setParameters").put("lifetime", "PERMANENT")
                .putArray("contributors").add("dhs-http").add("gsaoi");

        StringEntity input = new StringEntity(reqNode.toString());
        put.setEntity(input);
        put.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(put);
        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");
        assertNotNull("Respose did not response node.", responseNode);

        JsonNode status = responseNode.get("status");
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation failed.", "success", status.asText());
    }

    @Test
    public void testSetKeywords() throws ClientProtocolException, IOException {
        HttpClient client = HttpClientBuilder.create().build();
        String imageId = getImageWithParams();

        HttpPut put = new HttpPut(BASE_URI + "/" + imageId + "/keywords");
        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        ArrayNode keysArray = reqNode.with("setKeywords").put("final", false)
                .putArray("keywords");
        keysArray.addObject().put("name", "instrument").put("type", "STRING")
                .put("value", "GMOS-S");
        keysArray.addObject().put("name", "OBSERVAT").put("type", "STRING")
                .put("value", "Gemini-South");
        keysArray.addObject().put("name", "RA").put("type", "DOUBLE")
                .put("value", 162.31029167);
        keysArray.addObject().put("name", "PREIMAGE").put("type", "BOOLEAN")
                .put("value", false);
        keysArray.addObject().put("name", "NAMPS").put("type", "INT32")
                .put("value", 3);

        StringEntity input = new StringEntity(reqNode.toString());
        put.setEntity(input);
        put.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(put);

        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");

        JsonNode status = null;
        if (responseNode != null) {
            status = responseNode.get("status");
        }

        assertNotNull("Respose did not response node.", responseNode);
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation failed.", "success", status.asText());
    }

    private HttpResponse sendKeywords(String imageId) throws ClientProtocolException, IOException {
        HttpClient client = HttpClientBuilder.create().build();
    	HttpPut put = new HttpPut(BASE_URI + "/" + imageId + "/keywords");
        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        ArrayNode keysArray = reqNode.with("setKeywords").put("final", false)
                .putArray("keywords");
        keysArray.addObject().put("name", "instrument").put("type", "STRING")
                .put("value", "GMOS-S");
        keysArray.addObject().put("name", "OBSERVAT").put("type", "STRING")
                .put("value", "Gemini-South");
        keysArray.addObject().put("name", "RA").put("type", "DOUBLE")
                .put("value", 162.31029167);
        keysArray.addObject().put("name", "PREIMAGE").put("type", "BOOLEAN")
                .put("value", false);
        keysArray.addObject().put("name", "NAMPS").put("type", "INT32")
                .put("value", 3);

        StringEntity input = new StringEntity(reqNode.toString());
        put.setEntity(input);
        put.setHeader("Content-Type", "application/json");
        
        return client.execute(put);
    }
    
    @Test
    public void testSetKeywordsTwice() throws ClientProtocolException, IOException {

        String imageId = getImageWithParams();

        HttpResponse response = sendKeywords(imageId);

        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");

        JsonNode status = null;
        if (responseNode != null) {
            status = responseNode.get("status");
        }

        assertNotNull("Respose did not response node.", responseNode);
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation failed.", "success", status.asText());
        
        response = sendKeywords(imageId);

        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        mapper = new ObjectMapper();
        responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");

        status = null;
        if (responseNode != null) {
            status = responseNode.get("status");
        }

        assertNotNull("Respose did not response node.", responseNode);
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation failed.", "success", status.asText());
    }

    /*
     * Attempt to set string value to integer keyword. Rejected by the HTTP
     * service.
     */
    @Test
    public void testSetKeywordWrongValueType() throws ClientProtocolException,
            IOException {
        String imageId = getImageWithParams();
        HttpClient client = HttpClientBuilder.create().build();

        HttpPut put = new HttpPut(BASE_URI + "/" + imageId + "/keywords");
        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        ArrayNode keysArray = reqNode.with("setKeywords").put("final", false)
                .putArray("keywords");
        keysArray.addObject().put("name", "instrument").put("type", "STRING")
                .put("value", "GMOS-S");
        keysArray.addObject().put("name", "NAMPS").put("type", "INT32")
                .put("value", "dummy");

        StringEntity input = new StringEntity(reqNode.toString());
        put.setEntity(input);
        put.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(put);

        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");
        assertNotNull("Respose did not response node.", responseNode);

        JsonNode status = responseNode.get("status");
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation did not fail.", "error", status.asText());
    }

    /*
     * Attempt to set keyword with a type different from the one registered in
     * the DHS Server. HTTP Server must relay the error returned by the DHS
     * Server.
     */
    @Test
    public void testSetKeywordWrongType() throws ClientProtocolException,
            IOException {
        String imageId = getImageWithParams();
        HttpClient client = HttpClientBuilder.create().build();

        HttpPut put = new HttpPut(BASE_URI + "/" + imageId + "/keywords");
        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        ArrayNode keysArray = reqNode.with("setKeywords").put("final", false)
                .putArray("keywords");
        keysArray.addObject().put("name", "instrument").put("type", "STRING")
                .put("value", "GMOS-S");
        keysArray.addObject().put("name", "NAMPS").put("type", "STRING")
                .put("value", "dummy");

        StringEntity input = new StringEntity(reqNode.toString());
        put.setEntity(input);
        put.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(put);

        assertTrue("Operation rejected.", response.getStatusLine()
                .getStatusCode() == HttpStatus.SC_OK);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode responseNode = mapper.readTree(
                new InputStreamReader(response.getEntity().getContent())).get(
                "response");
        assertNotNull("Respose did not response node.", responseNode);

        JsonNode status = responseNode.get("status");
        assertNotNull("Respose did not contain status.", status);
        assertEquals("Operation did not fail.", "error", status.asText());
    }

    @Test
    public void testConcurrentCalls() throws InterruptedException,
            ExecutionException {
        final ConnectionRequester connectionRequester = new ConnectionRequester();

        final Callable<Boolean> task = new Callable<Boolean>() {
            @Override
            public Boolean call() {
                try {
                    return connectionRequester.requestConnection() != null;
                } catch (Exception e) {
                    isNotOk = true;
                }
                return false;
            }
        };

        final List<Callable<Boolean>> tasks = Collections.nCopies(
                numberOfThreads, task);
        final ExecutorService executorService = Executors
                .newFixedThreadPool(numberOfThreads);

        final List<Future<Boolean>> futures = executorService.invokeAll(tasks);
        executorService.shutdown();
        if (!executorService.awaitTermination(5, TimeUnit.SECONDS)) {
            isNotOk = true;
        }
        for (Future<Boolean> f : futures) {
            if (!(f.isDone() && f.get())) {
                isNotOk = true;
                break;
            }
        }

        assertFalse(isNotOk);
    }

    private String getImage() throws ClientProtocolException, IOException {
        HttpClient client = HttpClientBuilder.create().build();
        HttpPost post = new HttpPost(BASE_URI);

        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        reqNode.with("createImage");

        StringEntity input = new StringEntity(reqNode.toString());
        post.setEntity(input);
        post.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(post);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode resNode = mapper.readTree(new InputStreamReader(response
                .getEntity().getContent()));
        return resNode.get("response").get("result").asText();
    }

    private String getImageWithParams() throws ClientProtocolException,
            IOException {
        HttpClient client = HttpClientBuilder.create().build();
        HttpPost post = new HttpPost(BASE_URI);

        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        reqNode.with("createImage").put("lifetime", "PERMANENT")
                .putArray("contributors").add("dhs-http").add("gsaoi");

        StringEntity input = new StringEntity(reqNode.toString());
        post.setEntity(input);
        post.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(post);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode resNode = mapper.readTree(new InputStreamReader(response
                .getEntity().getContent()));
        return resNode.get("response").get("result").asText();
    }

    private class ConnectionRequester {

        public String requestConnection() throws ClientProtocolException,
                IOException {
            return getImage();
        }
    }

}
