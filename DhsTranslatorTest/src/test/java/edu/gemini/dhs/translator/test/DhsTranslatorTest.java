package edu.gemini.dhs.translator.test;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.io.InputStreamReader;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.HttpClientBuilder;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class DhsTranslatorTest {
    private final static String BASE_URI = "http://localhost:9090/axis2/services/dhs/images";

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
                .putArray("contributors").add("seqexec").add("gsaoi");

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

        HttpPut put = new HttpPut(
                "http://localhost:9090/axis2/services/dhs/images/" + imageId);
        ObjectNode reqNode = new JsonNodeFactory(true).objectNode();
        reqNode.with("setParameters").put("lifetime", "PERMANENT")
                .putArray("contributors").add("seqexec").add("gsaoi");

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

        HttpPut put = new HttpPut(
                "http://localhost:9090/axis2/services/dhs/images/" + imageId
                        + "/keywords");
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

    /*
     * Attempt to set string value to integer keyword. Rejected by the HTTP
     * service.
     */
    @Test
    public void testSetKeywordWrongValueType() throws ClientProtocolException,
            IOException {
        String imageId = getImageWithParams();
        HttpClient client = HttpClientBuilder.create().build();

        HttpPut put = new HttpPut(
                "http://localhost:9090/axis2/services/dhs/images/" + imageId
                        + "/keywords");
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

        HttpPut put = new HttpPut(
                "http://localhost:9090/axis2/services/dhs/images/" + imageId
                        + "/keywords");
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
                .putArray("contributors").add("seqexec").add("gsaoi");

        StringEntity input = new StringEntity(reqNode.toString());
        post.setEntity(input);
        post.setHeader("Content-Type", "application/json");
        HttpResponse response = client.execute(post);

        ObjectMapper mapper = new ObjectMapper();
        JsonNode resNode = mapper.readTree(new InputStreamReader(response
                .getEntity().getContent()));
        return resNode.get("response").get("result").asText();
    }

}
