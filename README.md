# DHS HTTP Service
## Introduction
DHS is the Data Handling System used at the Gemini Observatory to manage the generation of images in FITS format. The systems that generate image data communicate with the DHS Server using the DRAMA Interprocess Message Passing System created by AAO. Support for DHS is offered by a static library (dhsClient) written in C, 32 bits only.

This software module implements an HTTP service that wraps the dhsClient library, and allows to interact with the DHS Server using HTTP requests for a subset of the DHS methods.

This software uses the Apache Axis2/C framework. Specifically, it uses the axis2c-unofficial branch, which fixes several bugs in the last official release (1.6.0) and adds support for JSON payloads.

## Building
This module requires the following modules to build:
* dhsClient-devel (available in the Gemini Production RPM repository).
* axis2c-unofficial compiled with JSON support (package axis2c-unofficial-devel is available in the Gemini Development repository.).
* boost-devel (available in the CentOS repository).

Build the module from the DhsTranslator folder:

`make`

The Makefile assumes that the environment variables DHS\_CLIENT and AXIS2C\_HOME are defined to point to the dhsClient and axis2c installation base, respectively.

## Testing
First, prepare the test environment. From the DhsTranslator folder:

`make build-test`

Start the service:

`./runtest.sh`

The HTTP server will produce a log in DhsTranslator/server/logs/axis2.log.

The tests are in the folder DhsTranslatorTest. They are written in Java, and they are built using Maven.
To run the tests, from folder DhsTranslatorTest:

`mvn test`

## HTTP API
The HTTP API follows the REST style. The DHS operations always return the code 200. Other codes can be generated by the Axis2/C framework.

The DHS operations denote success of failure in the response body. An error response follows this model:

```
{ "response" : {
        "status" : "error"
        "errors" : [ {
             "type" : string /* error type, can be  BAD\_REQUEST | DHS\_ERROR | INTERNAL\_SERVER\_ERROR */
             "message" : string /* error message */
        } ]
    }
}
```

An successful response follow this model

```
{ "response" : {
        "status" : "success"
        "result" : /* depends on the operation, can be null */
    }
}
```

The defined operations are:

### createImage

HTTP method: POST

URI: http://{server}:9090/axis2/services/dhs/images

Body:
```
{ "createImage" : {
        "lifetime" : /* DHS lifetime, optional */
        "contributors" : [ string ] /* array of contributors, optional */
    }
}
```

Response: The response contains the unique id for the new image.
```
{ "response" : {
        "status" : "success"
        "result" : string /* The new image id */
    }
}
```

### setParameters

HTTP method: PUT

URI: http://{server}:9090/axis2/services/dhs/images/{id}

Body:
```
{ "setParameters" : {
        "lifetime" : /* DHS lifetime, optional */
        "contributors" : [ string ] /* array of contributors, optional */
    }
}
```

Response: result is null.

### setKeywords

HTTP method: PUT

URI: http://{server}:9090/axis2/services/dhs/images/{id}/keywords

Body:
```
{ "setKeywords" : { 
    "final" : boolean /* indicates if this is the last piece of data for the image */
    "keywords" : [ {
        "name" : string /* keyword name */
        "type" : string /* data type, can be INT8 | INT16 | INT32 | UINT8 | UINT16 | UINT32 | FLOAT | DOUBLE | BOOLEAN | STRING
        "value" : /* keyword value, type must agree with the type field */
    } ]
}
```

Response: result is null.
