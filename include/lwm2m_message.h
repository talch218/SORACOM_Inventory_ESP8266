#include <stdlib.h>

static const char* __state[] = {
    "STATE_INITIAL",
    "STATE_BOOTSTRAP_REQUIRED",
    "STATE_BOOTSTRAPPING",
    "STATE_REGISTER_REQUIRED",
    "STATE_REGISTER_REQUIRED2",
    "STATE_REGISTERING",
    "STATE_READY",
    "STATE_EXCEPTIONAL",
};

static const char* __error[] = {
    "COAP_NO_ERROR",
    "COAP_IGNORE",
    "COAP_CONNECT_FAIL",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL,
    "COAP_201_CREATED",
    "COAP_202_DELETED",
    "COAP_204_CHANGED",
    "COAP_205_CONTENT",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "COAP_231_CONTINUE",

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    "COAP_400_BAD_REQUEST",
    "COAP_401_UNAUTHORIZED",
    "COAP_402_BAD_OPTION",
    "COAP_403_FORBIDDEN",
    "COAP_404_NOT_FOUND",
    "COAP_405_METHOD_NOT_ALLOWED",
    "COAP_406_NOT_ACCEPTABLE",
    NULL,
    "COAP_408_REQ_ENTITY_INCOMPLETE",
    NULL, NULL, NULL,
    "COAP_412_PRECONDITION_FAILED",
    "COAP_413_ENTITY_TOO_LARGE",
    NULL, NULL,

    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    "COAP_500_INTERNAL_SERVER_ERROR",
    "COAP_501_NOT_IMPLEMENTED",
    NULL,
    "COAP_503_SERVICE_UNAVAILABLE"};
