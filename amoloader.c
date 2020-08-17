#include "amoloader.h"

struct amoloader_data *amo_load(const char *path, int *count)
{
    FILE *file;
    struct amoloader_data *data;
    char cmd_buf[256];
    int vertex_count = 0;
    int normal_count = 0;
    int uv_count = 0;
    int joint_count = 0;
    int weight_count = 0;
    int o_index = 0;
    int i;

    *count = 0;

    /* Check if file is a .amo file */
    if(strcmp(strrchr(path, '.'), ".amo") != 0) {
        return NULL;
    }
    /* Open file and initialize data struct */
    file = fopen(path, "r");
    if(!file) {
        return NULL;
    }
    data = calloc(sizeof(struct amoloader_data), 1);
    if(data == NULL) {
        fclose(file);
        return NULL;
    }

    /* Go through every line and read the first word.
     * If its a valid amo keyword read the arguments and put them into the right
     * position in the data struct */
    while(fscanf(file, "%s", cmd_buf) != EOF) {
        /* o <name> */
        if(strcmp(cmd_buf, "o") == 0 || strcmp(cmd_buf, "ao") == 0) {
            *count+= 1;
            o_index = *count - 1;
            printf("%d\n", o_index);
            if((*count) > 1) {
                data = realloc(data, sizeof(struct amoloader_data)*(*count));
            }
            printf("Test\n");
            fscanf(file, "%s", data[o_index].name);
            printf("Test1\n");
        }
        /* v <x> <y> <z> */
        if(strcmp(cmd_buf, "v") == 0) {
            data[o_index].vertex_buffer = realloc(data[o_index].vertex_buffer,
                                          sizeof(float)*3*(vertex_count+1));
            fscanf(file, "%f %f %f", &data[o_index].vertex_buffer[vertex_count*3],
                                     &data[o_index].vertex_buffer[vertex_count*3+1],
                                     &data[o_index].vertex_buffer[vertex_count*3+2]);
            vertex_count++;
        /* vn <x> <y> <z> */
        } else if(strcmp(cmd_buf, "vn") == 0) {
            data[o_index].normal_buffer = realloc(data[o_index].normal_buffer,
                                          sizeof(float)*3*(normal_count+1));
            fscanf(file, "%f %f %f", &data[o_index].normal_buffer[normal_count*3],
                                     &data[o_index].normal_buffer[normal_count*3+1],
                                     &data[o_index].normal_buffer[normal_count*3+2]);
            normal_count++;
        /* vt <x> <y> */
        } else if(strcmp(cmd_buf, "vt") == 0) {
            data[o_index].uv_buffer = realloc(data[o_index].uv_buffer,
                                      sizeof(float)*2*(uv_count+1));
            fscanf(file, "%f %f", &data[o_index].uv_buffer[uv_count*2],
                                  &data[o_index].uv_buffer[uv_count*2+1]);
            uv_count++;
        /* vj <joint_1> <joint_2> <joint_3> <joint_4> */
        } else if(strcmp(cmd_buf, "vj") == 0) {
            data[o_index].joint_buffer = realloc(data[o_index].joint_buffer,
                                         sizeof(float)*4*(joint_count+1));
            fscanf(file, "%d %d %d %d", &data[o_index].joint_buffer[joint_count*4],
                                        &data[o_index].joint_buffer[joint_count*4+1],
                                        &data[o_index].joint_buffer[joint_count*4+2],
                                        &data[o_index].joint_buffer[joint_count*4+3]);
            joint_count++;
        /* vw <weight_1> <weight_2> <weight_3> <weight_4> */
        } else if(strcmp(cmd_buf, "vw") == 0) {
            data[o_index].weight_buffer = realloc(data[o_index].weight_buffer,
                                          sizeof(float)*4*(weight_count+1));
            fscanf(file, "%f %f %f %f", &data[o_index].weight_buffer[weight_count*4],
                                        &data[o_index].weight_buffer[weight_count*4+1],
                                        &data[o_index].weight_buffer[weight_count*4+2],
                                        &data[o_index].weight_buffer[weight_count*4+3]);
            weight_count++;
        /* f <pos>/<normal>/<uv>/<joint>/<weight>
         *   <pos>/<normal>/<uv>/<joint>/<weight>
         *   <pos>/<normal>/<uv>/<joint>/<weight> 
         * Currently, the library expects <pos>/<normal>/<uv>/<joint>/<weight>
         * to be the same value, since editamo exports it like that.
         * Definetly needs to be improved */
        } else if(strcmp(cmd_buf, "f") == 0) {
            data[o_index].index_buffer = realloc(data[o_index].index_buffer,
                                sizeof(unsigned int)*3*(data[o_index].indices_count+1));
            fscanf(file, "%u/%u/%u/%u/%u %u/%u/%u/%u/%u %u/%u/%u/%u/%u",
                         &data[o_index].index_buffer[data[o_index].indices_count*3],
                         &data[o_index].index_buffer[data[o_index].indices_count*3],
                         &data[o_index].index_buffer[data[o_index].indices_count*3],
                         &data[o_index].index_buffer[data[o_index].indices_count*3],
                         &data[o_index].index_buffer[data[o_index].indices_count*3],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+1],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+1],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+1],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+1],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+1],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+2],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+2],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+2],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+2],
                         &data[o_index].index_buffer[data[o_index].indices_count*3+2]);
            /* obj starts counting at 1 */
            data[o_index].index_buffer[data[o_index].indices_count*3]--;
            data[o_index].index_buffer[data[o_index].indices_count*3+1]--;
            data[o_index].index_buffer[data[o_index].indices_count*3+2]--;

            data[o_index].indices_count++;
        /* j <name> <parent> */
        } else if(strcmp(cmd_buf, "j") == 0) {
            int joint;
            int parent_joint;
            data[o_index].joints_count++;
            data[o_index].joints = realloc(data[o_index].joints,
                            sizeof(struct amoloader_joint)*data[o_index].joints_count);
            fscanf(file, "%s %d", data[o_index].joints[data[o_index].joints_count-1].name,
                                  &parent_joint);
            /* Special treatment for root joint */
            joint = data[o_index].joints_count-1;
            if(parent_joint == -1) {
                data[o_index].joints[joint].parent = NULL;
            } else {
                data[o_index].joints[joint].parent = &data[o_index].joints[parent_joint-1];
            }
        /* a <name> */
        } else if(strcmp(cmd_buf, "a") == 0) {
            data[o_index].animations_count++;
            data[o_index].animations = realloc(data[o_index].animations,
                    sizeof(struct amoloader_animation)*data[o_index].animations_count);
            fscanf(file, "%s", data[o_index].animations[data[o_index].animations_count-1].name);
        /* ap <timestamp> <joint> <x> <y> <z> */
        } else if(strcmp(cmd_buf, "ap") == 0) {
            struct amoloader_keyframe_pos *keyframe;
            int joint;
            struct amoloader_animation *animation;
            animation = &data[o_index].animations[data[o_index].animations_count-1];
            animation->keyframes_pos_num++;
            animation->keyframes_pos = realloc(animation->keyframes_pos,
            sizeof(struct amoloader_keyframe_pos)*animation->keyframes_pos_num);
            keyframe =&animation->keyframes_pos[animation->keyframes_pos_num-1];
            fscanf(file, "%f %d %f %f %f", &keyframe->timestamp, &joint,
                                           &keyframe->pos[0],
                                           &keyframe->pos[1],
                                           &keyframe->pos[2]);
            keyframe->joint = &data[o_index].joints[joint-1];
        /* ar <timestamp> <joint> <x> <y> <z> <w> */
        } else if(strcmp(cmd_buf, "ar") == 0) {
            struct amoloader_keyframe_rot *keyframe;
            int joint;
            struct amoloader_animation *animation;
            animation = &data[o_index].animations[data[o_index].animations_count-1];
            animation->keyframes_rot_num++;
            animation->keyframes_rot = realloc(animation->keyframes_rot,
            sizeof(struct amoloader_keyframe_rot)*animation->keyframes_rot_num);
            keyframe =&animation->keyframes_rot[animation->keyframes_rot_num-1];
            fscanf(file, "%f %d %f %f %f %f", &keyframe->timestamp, &joint,
                                              &keyframe->rot[0],
                                              &keyframe->rot[1],
                                              &keyframe->rot[2],
                                              &keyframe->rot[3]);
            keyframe->joint = &data[o_index].joints[joint-1];
        }
    }
    /* This is the part, where the logic to order the buffer based on f should
     * come in.
     * Needs to be implemented */
    for(i = 0; i < *count; i++) {
        data[i].vertices_count = vertex_count;
    }

    fclose(file);
    return data;
}

int amo_destroy(struct amoloader_data *data, int count)
{
    int i, j;

    if(data == NULL) {
        return -1;
    }
    for(i = 0; i < count; i++) {
        free(data[i].vertex_buffer);
        free(data[i].normal_buffer);
        free(data[i].uv_buffer);
        if(data[i].joints_count != 0) {
            free(data[i].joint_buffer);
            free(data[i].weight_buffer);
            free(data[i].joints);
            for(j = 0; j < data[i].animations_count; j++) {
                free(data[i].animations[j].keyframes_pos);
                free(data[i].animations[j].keyframes_rot);
            }
            free(data[i].animations);
        }
    }
    free(data);
    return 0;
}