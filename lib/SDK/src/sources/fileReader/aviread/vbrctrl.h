/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef VBRCRTL_H
#define VBRCRTL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * for internal usage only!
 */

//private strutures
struct vbrctrl_s;
typedef struct vbrctrl_s vbrctrl_t;

// cripsness is ignored at this moment
vbrctrl_t* vbrctrl_init_1pass(int quality, int crispness);
vbrctrl_t* vbrctrl_init_2pass_analysis(const char* filename,
				       int quality, int crispness);
vbrctrl_t* vbrctrl_init_2pass_encoding(const char* filename, int bitrate,
				       double framerate, int quality,
				       int crispness);
void vbrctrl_close(vbrctrl_t*);

void vbrctrl_update_1pass(vbrctrl_t*);
void vbrctrl_update_2pass_analysis(vbrctrl_t*,
				   int is_key_frame, int motion_bits,
				   int texture_bits, int total_bits, int quant);
void vbrctrl_update_2pass_encoding(vbrctrl_t*, int motion_bits,
				   int texture_bits, int total_bits);

int vbrctrl_get_drop(vbrctrl_t*);
int vbrctrl_get_intra(vbrctrl_t*);
int vbrctrl_get_quant(vbrctrl_t*);
void vbrctrl_set_quant(vbrctrl_t*, float q);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VBRCRTL_H */
