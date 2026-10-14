/*
 * Copyright (C) 2023-2025 Rockchip Corp. All Rights Reserved.
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * Rockchip Corporation ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with Rockchip Corporation.
 *
 * Unauthorized copying, distribution, or use of this software, via any medium,
 * is strictly prohibited. This software is protected by applicable copyright
 * laws and international treaties.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------- function declaration ---------------------------------------*/
/*
    * Create a new hci instance with the json para files.
    * para_file: Path to the json parameter file for the hci instance.
    * Returns a pointer to the created hci instance, or NULL on failure.
*/
extern void* rk_audio_hci_init(const char *conf_path);

/*
    * Destroy a previously created hci instance.
    * ptr_hci: Pointer to the hci instance to be destroyed.
*/
extern void rk_audio_hci_destory(void* ptr_hci);

/*
    * Process a frame of audio data through the hci instance.
    * data_in: Pointer to the input short int microphone data and reference data, interleaved format.
    * data_out: Pointer to the output short int buffer for processed data.
    * data_size: bytes length of data_in
    * ptr_hci: Pointer to the hci instance to use for processing.
    * Returns bytes length of data_out
*/
extern int rk_audio_hci_process(const short* data_in, short* data_out, int data_size, void* ptr_hci);

/*
    * Get rk hci version.
    * ptr_hci: Pointer to the hci instance.
    * Returns version string.
*/
extern const char* rk_audio_hci_get_version(void *ptr_hci);

/*
    * Set noise reduction level through the hci instance.
    * ptr_hci: Pointer to the hci instance.
    * level: 0 for disable traditional-nr and AI-nr; 1 for only enable traditional-nr; 2 for only enable AI-nr
    * Returns code, 0 for success, -1 means failure.
*/
extern int rk_audio_hci_set_anr_level(void *ptr_hci, int level);

/*
    * Get vad of the current frame through the hci instance.
    * ptr_hci: Pointer to the hci instance to use for processing.
    * Returns vad, value in [0, 1]
 */
extern int rk_audio_hci_get_vad(void *ptr_hci);

/*
    * Get DOA through the hci instance.
    * ptr_hci: Pointer to the hci instance.
    * Returns DOA, value range from 0 to 180 for linear array and from 0 to 360 for circular array, -1 means no speech
 */
extern int rk_audio_hci_get_doa(void* ptr_hci);

/*
    * Get rk hci wakeup status.
    * kws_doa: pointer to wakeup direction.
    * ptr_hci: Pointer to the hci instance.
    * Returns wakeup status, 0 means no wakeup, 1 means wakup word 1, 2 means wakeup word 2, etc.
*/
extern int rk_audio_hci_get_wakeup(int *kws_doa, void* ptr_hci);

/*
    * Set rk hci kws level. If call this functionn, must set kws_level to 1 in json for init.
    * kws_level: 0 means diable kws, 1 means enable kws.
    * ptr_hci: Pointer to the hci instance.
    * Returns set status, 0 succeed, 1 failure.
*/
extern int rk_audio_hci_set_wakeup(int kws_level, void* ptr_hci);

/*
    * Get rk hci scene detection(sed) status.
    * ptr_hci: Pointer to the hci instance.
    * Returns sed status, 0 means nothing detected, 1 means dog barking detected, 2 means baby crying detected.
*/
extern int rk_audio_hci_get_sed(void* ptr_hci);

/*
    * Get Post audio data through the hci instance. Must be called after rk_audio_hci_process();
    * data_out: Pointer to the output short int buffer with post process. The length of data_out here is same as the data_out in rk_audio_hci_process();
    * ptr_hci: Pointer to the hci instance to use for processing.
    * Returns bytes length of data_out, -1 means failure.
*/
extern int rk_audio_hci_post_out(short* data_out, void* ptr_hci);

#ifdef __cplusplus
}
#endif

