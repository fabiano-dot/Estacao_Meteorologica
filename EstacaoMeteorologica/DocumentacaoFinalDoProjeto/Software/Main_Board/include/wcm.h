/**
 * @file    wcm.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Comunica com o WCM enviado os comandos para envio das mensagens por LoRa
 * @version 0.1
 * @date    2026-02-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef WCM_H
#define WCM_H

#include "aq_data.h"
#include "est_config.h"


/**
 * @brief Inicializa a UART utilizada para comunicação com o WCM
 * 
 */
void wcm_init();

/**
 * @brief Gera os comandos para o WCM e aguarda resposta
 *        - Monta o comando, sem os dados
 *        - Adiciona o payload dos dados adquiridos
 *        - Acorda o WCM
 *        - Limpa o buffer de recepção
 *        - Envia o comando
 *        - Aguarda o retorno do WCM;
 * 
 * @param est_config Contêm os parâmetros para a transmissão
 * @param aq_data Contêm os dados que serão transmitidos
 */
void wcm_send(EstConfig *est_config, AqData *aq_data);

#endif // WCM_H