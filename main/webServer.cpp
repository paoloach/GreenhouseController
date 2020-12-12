//
// Created by paolo on 12/9/20.
//

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <ff.h>
#include "include/webServer.h"
#include "include/Settings.h"
#include "include/GroupSignals.h"


WebServer webServer;
constexpr  int BUFFER_SIZE=1024;

static esp_err_t methodNotSupported(httpd_req_t *connData) {
    const char *msg = "Request method is not supported by server\n";
    const char *status = "501 Method Not Implemented\n";
    httpd_resp_set_status(connData, status);
    httpd_resp_set_type(connData, HTTPD_TYPE_TEXT);
    return httpd_resp_send(connData, msg, strlen(msg));
}


esp_err_t WebServer::postOTA(httpd_req_t *req) {
    char buffer[BUFFER_SIZE];
    esp_ota_handle_t updateHandle;

    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == nullptr){
        ESP_LOGE(TAG, "Update partition fail");
        httpd_resp_send_500(req);
        return  ESP_OK;
    }
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);


    uint32_t readTot=0;
    bool otaStarted=false;
    while(readTot < req->content_len){
        size_t recv_size = MIN(req->content_len, sizeof(BUFFER_SIZE));
        int ret = httpd_req_recv(req, buffer, recv_size);
        if (ret == HTTPD_SOCK_ERR_TIMEOUT )
            continue;
        if (ret< 0){
            ESP_LOGE(TAG, "OTA read failed (%s)", esp_err_to_name(ret));
            httpd_resp_send_500(req);
            return  ESP_FAIL;
        }
        int dataRead = ret;
        readTot += dataRead;
        ESP_LOGI(TAG,"Read %d bytes until now",readTot );
        if (!otaStarted){
            esp_err_t err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &updateHandle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                httpd_resp_send_500(req);
                return  ESP_FAIL;
            }
            otaStarted=true;
        }
        esp_ota_write(updateHandle, buffer, dataRead);

    }
    if (esp_ota_end(updateHandle) != ESP_OK){
        ESP_LOGE(TAG, "OTA end error ");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    if (esp_ota_set_boot_partition(update_partition) != ESP_OK){
        ESP_LOGE(TAG, "OTA boot partition error ");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG,"OTA update successfully");
    httpd_resp_send(req, "OTA update successfully\n", -1);

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
