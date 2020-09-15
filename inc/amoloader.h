/*
 * zlib License
 * 
 * (C) 2020 VasallSoftware
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 */

#ifndef _AMOLOADER_H
#define _AMOLOADER_H

#define AMO_API extern

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum amo_format {
	AMO_FORMAT_OBJ,
	AMO_FORMAT_AMO
};

/*
 * A struct symbolising a joint of the model.
 * 
 * @name: The name of the joint
 * @par: A pointer to the parent joint
 */
struct amo_joint;
struct amo_joint {
    char              name[100];
    int               index;
    struct amo_joint  *par;
};

struct amo_keyfr {
	float ts;

	struct amo_joint  **joints;
	float             *pos;
	float             *rot;
};

/*
 * A struct containing information about one animation cycle
 * 
 * @name: The name of the animation
 * @keyfr_pos: An array of keyframes for the position of the joints
 * @keyfr_pos_num: The number of keyframes for the position
 * @keyfr_rot: An array of keyframes for the rotation of the joints
 * @keyfr_rot_num: The numbegTr of keyframes for the rotation
 */
struct amo_anim {
    char                  name[100];

    int                   keyfr_c;
    struct amo_keyfr      *keyfr_lst;
};

/*
 * A struct containing the parsed mdl of the amo file
 * 
 * @name: The name of the object
 * @vtx_c: The amount of vertices
 * @vtx_buf: A 3-dimensional array of vertex positions
 * @tex_buf: A 2-dimensional array of uv coordinates
 * @nrm_buf: A 3-dimensional array of vertex normals
 * @jnt_buf: A 4-dimensional array of joints affecting the vertices
 * @wgt_buf: A 4-dimensional array of weights affecting the vertices
 * @idx_c: The amount of indices
 * @idx_buf: An array of indices
 * @jnt_c: The amount of joints
 * @jnt_lst: An array of the joints of the model
 * @ani_c: The amount of animations
 * @ani_lst: An array of animations of the model
 */
struct amo_model {
    char              name[100];
    enum amo_format   format;

    int               vtx_c;
    float             *vtx_buf;

    int               tex_c;
    float             *tex_buf;

    int               nrm_c;
    float             *nrm_buf;

    int               *vjnt_buf;
    float             *wgt_buf;

    int               idx_c;
    unsigned int      *idx_buf;

    int               jnt_c;
    struct amo_joint  *jnt_lst;
    
    int               ani_c;
    struct amo_anim   *ani_lst;
};

/*
 * Loads the .amo file and parses its values into amo_model
 * 
 * @pth: The file path to the dot-amo file
 * 
 * Returns: Returns the array containing the models read from the given dot-amo
 *          file
 */
AMO_API struct amo_model *amo_load(const char *pth);


/*
 * Destroys the amo_model struct
 * 
 * @data: An array of models previously created by amo_load()
 * 
 * Returns: 0 for success or -1 if an error occurred
 */
AMO_API void amo_destroy(struct amo_model *data);


/*
 *  
 */
AMO_API int amo_getmesh(struct amo_model *data, int *vtxnum,
		void **vtx, void **tex, void **nrm, int *idxnum,
		unsigned int **idx);

/*
 *    &*                    #@,        
 *   #@@%                  @@@&        
 *   @@@@@#@@@@@@@@@@@@@@@@@@@@        
 *   &@@@@@@@@@@@@@@@@@@@@@@@@%        
 *   /@@@@@@@@@@@@@@@@@@@@@@@@@@&      
 *  %@@@@*   *@@@@@@@(   .@@@@@@@@*    
 * #@@@&       %@@@&       *@@@@@@@,   
 * @@@@@       @@@@@       %@@@@@@@@   
 * @@@@@@@%%%@@@@@@@@@&%%@@@@@@@@@@@.  
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.  
 * ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
 *  /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
 *    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
 *      @@@@@@@@@@@@@@@@@@@@@@@@@@@@   
 *        &@@@@@@@@@@@@@@@@@@@@@@@@@%  
 *          (@@@@@@@@@@@@@@@@@.&@@@@@& 
 *             ,@@@@@@@@@@@@@@#    .,,.
 *                 ,@@@@@@@@@@@@       
 *                       #&@@@@@@,     
 *                               .     
 *
 * Wanna see something good?
 * https://youtu.be/dQw4w9WgXcQ
 */

#endif
