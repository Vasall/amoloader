/*
 * zlib License
 * 
 * (C) 2020 enudstudios
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

#ifndef AMOLOADER_H
#define AMOLOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * A struct symbolising a joint of the model
 * 
 * @name: The name of the joint
 * @parent: A pointer to the parent joint
 */
typedef struct amoloader_joint {
    char name[100];
    struct amoloader_joint* parent;
} amoloader_joint;

/*
 * A struct containing information about a keyframe, which alters the position of a joint
 * 
 * @timestamp: The timestamp at which the joint should have this position
 * @joint: A pointer to the affected joint
 * @pos: 3 floats containing the position the joint should have at this keyframe
 */
typedef struct amoloader_animation_keyframe_pos {
    float timestamp;
    amoloader_joint* joint;
    float pos[3];
} amoloader_animation_keyframe_pos;

/*
 * A struct containig information about a keyframe, which alters the rotation of a joint
 * 
 * @timestamp: The timestamp at which the joint should have this rotation
 * @joint: A pointer to the affected joint
 * @rot: 4 floats containig the rotation in quaternions the joint should have at this keyframe
 */
typedef struct amoloader_animation_keyframe_rot {
    float timestamp;
    amoloader_joint* joint;
    float rot[4];
} amoloader_animation_keyframe_rot;

/*
 * A struct containing information about one animation cycle
 * 
 * @name: The name of the animation
 * @keyframes_pos: An array of keyframes for the position of the joints
 * @keyframes_pos_count: The number of keyframes for the position
 * @keyframes_rot: An array of keyframes for the rotation of the joints
 * @keyframes_rot_count: The number of keyframes for the rotation
 */
typedef struct amoloader_animation {
    char name[100];
    amoloader_animation_keyframe_pos* keyframes_pos;
    int keyframes_pos_count;
    amoloader_animation_keyframe_rot* keyframes_rot;
    int keyframes_rot_count;
} amoloader_animation;

/*
 * A struct containing the parsed data of the amo file
 * 
 * @vertices_count: The amount of vertices
 * @vertex_buffer: A 3-dimensional array of vertex positions
 * @normal_buffer: A 3-dimensional array of vertex normals
 * @uv_buffer: A 2-dimensional array of uv coordinates
 * @joint_uffer: A 4-dimensional array of joints affecting the vertices
 * @weight_buffer: A 4-dimensional array of weights affecting the vertices
 * @indices_count: The amount of indices
 * @index_buffer: An array of indices
 * @joints_count: The amount of joints
 * @amoloader_joints: An array of the joints of the model
 * @animations_count: The amount of animations
 * @amoloader_animations: An array of animations of the model
 */
typedef struct amoloader_data {
    int vertices_count;
    float* vertex_buffer;
    float* normal_buffer;
    float* uv_buffer;
    int* joint_buffer;
    float* weight_buffer;
    int indices_count;
    unsigned int* index_buffer;
    int joints_count;
    amoloader_joint* joints;
    int animations_count;
    amoloader_animation* animations;
} amoloader_data;

/*
 * Loads the .amo file and parses its values into amoloader_data
 * 
 * @path: The file path to the .amo file
 * 
 * Returns: A pointer to an amoloader_data struct containing the parsed data or NULL if an error occurred
 */
amoloader_data* loadAMO(const char* path);

/*
 * Destroys the amoloader_data struct
 * 
 * @data: the amoloader_data struct that was recieved by loadAMO()
 * 
 * Returns: 0 for success or -1 if an error occurred
 */
int destroyAMO(amoloader_data* data);

/*
     &*                    #@,        
    #@@%                  @@@&        
    @@@@@#@@@@@@@@@@@@@@@@@@@@        
    &@@@@@@@@@@@@@@@@@@@@@@@@%        
    /@@@@@@@@@@@@@@@@@@@@@@@@@@&      
   %@@@@*   *@@@@@@@(   .@@@@@@@@*    
  #@@@&       %@@@&       *@@@@@@@,   
  @@@@@       @@@@@       %@@@@@@@@   
  @@@@@@@%%%@@@@@@@@@&%%@@@@@@@@@@@.  
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.  
  ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
   /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
       @@@@@@@@@@@@@@@@@@@@@@@@@@@@   
         &@@@@@@@@@@@@@@@@@@@@@@@@@%  
           (@@@@@@@@@@@@@@@@@.&@@@@@& 
              ,@@@@@@@@@@@@@@#    .,,.
                  ,@@@@@@@@@@@@       
                        #&@@@@@@,     
                                .     
 */

#endif