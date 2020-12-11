//
// Created by paolo on 12/9/20.
//

#include <esp_http_server.h>
#include <esp_log.h>
#include "include/webServer.h"
#include "include/Settings.h"
#include "include/GroupSignals.h"


WebServer webServer;

static esp_err_t methodNotSupported(httpd_req_t *connData) {
    const char *msg = "Request method is not supported by server\n";
    const char *status = "501 Method Not Implemented\n";
    httpd_resp_set_status(connData, status);
    httpd_resp_set_type(connData, HTTPD_TYPE_TEXT);
    return httpd_resp_send(connData, msg, strlen(msg));
}


esp_err_t WebServer::postOTA(httpd_req_t *req) {
    return ESP_OK;
}
esp_err_t WebServer::setSampleIntervalHandler(httpd_req_t *req) {
    return ESP_OK;
}
esp_err_t WebServer::getStatusHandler(httpd_req_t *req) {

    auto state = cJSON_CreateObject();
    for (uint8_t i = 0; i < totSensors; i++) {
        sensors[i]->setState(state);
    }
    auto data = cJSON_Print(state);
    httpd_resp_set_type(req, HTTPD_TYPE_JSON);
    httpd_resp_send(req, data, -1);

    cJSON_Delete(state);

    return ESP_OK;
}
esp_err_t WebServer::aboutHandler(httpd_req_t *req) {
    if (req->method != HTTP_GET) {
        return methodNotSupported(req);
    }
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    char buffer[100];
    sprintf(buffer, "%s\n", Settings::settings.name);
    httpd_resp_send(req, buffer, -1);

    return ESP_OK;
}

void initWebserver() {
    webServer.init();

}

void WebServer::init() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 5;

    httpd_uri_t getAbout = {.uri = "/about", .method = HTTP_GET, .handler = aboutHandler, .user_ctx = NULL};
    httpd_uri_t getStatus = {.uri = "/status", .method = HTTP_GET, .handler = getStatusHandler, .user_ctx = NULL};
    httpd_uri_t postOta = {.uri = "/ota", .method = HTTP_POST, .handler = postOTA, .user_ctx = NULL};
    httpd_uri_t setSampleInterval = {.uri = "/sampleInterval", .method = HTTP_POST, .handler = setSampleIntervalHandler, .user_ctx = NULL};


    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE,portMAX_DELAY );

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&httpServer, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(httpServer, &getAbout);
        httpd_register_uri_handler(httpServer, &getStatus);
        httpd_register_uri_handler(httpServer, &setSampleInterval);
        httpd_register_uri_handler(httpServer, &postOta);
        ESP_LOGI(TAG, "HTTP SERVER STARTED");
        return;
    }

    ESP_LOGI(TAG, "Error starting server!");
}
