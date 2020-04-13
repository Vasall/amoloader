#include "amoloader.h"

amoloader_data* loadAMO(const char* path) {
    FILE* file;
    amoloader_data* data;
    char cmd_buf[256];
    int vertex_count = 0;
    int normal_count = 0;
    int uv_count = 0;
    int joint_count = 0;
    int weight_count = 0;

    if(!strstr(path, ".amo")) {
        return NULL;
    }
    file = fopen(path, "r");
    if(!file) {
        return NULL;
    }
    data = calloc(sizeof(amoloader_data), 1);
    if(data == NULL) {
        fclose(file);
        return NULL;
    }

    while(fscanf(file, "%s", cmd_buf) != EOF) {
        if(strcmp(cmd_buf, "v") == 0) {
            data->vertex_buffer = realloc(data->vertex_buffer, sizeof(float)*3*(vertex_count+1));
            fscanf(file, "%f %f %f", &data->vertex_buffer[vertex_count*3],
                                     &data->vertex_buffer[vertex_count*3+1],
                                     &data->vertex_buffer[vertex_count*3+2]);
            vertex_count++;
        } else if(strcmp(cmd_buf, "vn") == 0) {
            data->normal_buffer = realloc(data->normal_buffer, sizeof(float)*3*(normal_count+1));
            fscanf(file, "%f %f %f", &data->normal_buffer[normal_count*3],
                                     &data->normal_buffer[normal_count*3+1],
                                     &data->normal_buffer[normal_count*3+2]);
            normal_count++;
        } else if(strcmp(cmd_buf, "vt") == 0) {
            data->uv_buffer = realloc(data->uv_buffer, sizeof(float)*2*(uv_count+1));
            fscanf(file, "%f %f", &data->uv_buffer[uv_count*2],
                                  &data->uv_buffer[uv_count*2+1]);
            uv_count++;
        } else if(strcmp(cmd_buf, "vj") == 0) {
            data->joint_buffer = realloc(data->joint_buffer, sizeof(float)*4*(joint_count+1));
            fscanf(file, "%d %d %d %d", &data->joint_buffer[joint_count*4],
                                        &data->joint_buffer[joint_count*4+1],
                                        &data->joint_buffer[joint_count*4+2],
                                        &data->joint_buffer[joint_count*4+3]);
            joint_count++;
        } else if(strcmp(cmd_buf, "vw") == 0) {
            data->weight_buffer = realloc(data->weight_buffer, sizeof(float)*4*(weight_count+1));
            fscanf(file, "%f %f %f %f", &data->weight_buffer[weight_count*4],
                                        &data->weight_buffer[weight_count*4+1],
                                        &data->weight_buffer[weight_count*4+2],
                                        &data->weight_buffer[weight_count*4+3]);
            weight_count++;
        } else if(strcmp(cmd_buf, "f") == 0) {
            data->index_buffer = realloc(data->index_buffer, sizeof(unsigned int)*3*(data->indices_count+1));
            fscanf(file, "%u/%u/%u/%u/%u %u/%u/%u/%u/%u %u/%u/%u/%u/%u", &data->index_buffer[data->indices_count*3],
                                                                         &data->index_buffer[data->indices_count*3],
                                                                         &data->index_buffer[data->indices_count*3],
                                                                         &data->index_buffer[data->indices_count*3],
                                                                         &data->index_buffer[data->indices_count*3],
                                                                         &data->index_buffer[data->indices_count*3+1],
                                                                         &data->index_buffer[data->indices_count*3+1],
                                                                         &data->index_buffer[data->indices_count*3+1],
                                                                         &data->index_buffer[data->indices_count*3+1],
                                                                         &data->index_buffer[data->indices_count*3+1],
                                                                         &data->index_buffer[data->indices_count*3+2],
                                                                         &data->index_buffer[data->indices_count*3+2],
                                                                         &data->index_buffer[data->indices_count*3+2],
                                                                         &data->index_buffer[data->indices_count*3+2],
                                                                         &data->index_buffer[data->indices_count*3+2]);
            data->index_buffer[data->indices_count*3]--;
            data->index_buffer[data->indices_count*3+1]--;
            data->index_buffer[data->indices_count*3+2]--;

            data->indices_count++;
        } else if(strcmp(cmd_buf, "j") == 0) {
            int parent_joint;
            data->joints_count++;
            data->joints = realloc(data->joints, sizeof(amoloader_joint)*data->joints_count);
            fscanf(file, "%s %d", data->joints[data->joints_count-1].name, &parent_joint);
            if(parent_joint == -1) {
                data->joints[data->joints_count-1].parent = NULL;
            } else {
                data->joints[data->joints_count-1].parent = &data->joints[parent_joint-1];
            }
        } else if(strcmp(cmd_buf, "a") == 0) {
            data->animations_count++;
            data->animations = realloc(data->animations, sizeof(amoloader_animation)*data->animations_count);
            fscanf(file, "%s", data->animations[data->animations_count-1].name);
        } else if(strcmp(cmd_buf, "ap") == 0) {
            amoloader_animation_keyframe_pos* keyframe;
            int joint;
            amoloader_animation* animation = &data->animations[data->animations_count-1];
            animation->keyframes_pos_count++;
            animation->keyframes_pos = realloc(animation->keyframes_pos, sizeof(amoloader_animation_keyframe_pos)*animation->keyframes_pos_count);
            keyframe = &animation->keyframes_pos[animation->keyframes_pos_count-1];
            fscanf(file, "%f %d %f %f %f", &keyframe->timestamp, &joint, &keyframe->pos[0], &keyframe->pos[1], &keyframe->pos[2]);
            keyframe->joint = &data->joints[joint-1];
        }
    }
    data->vertices_count = vertex_count;
    fclose(file);
    return data;
}

int destroyAMO(amoloader_data* data) {
    int i;

    if(data == NULL) {
        return -1;
    }
    free(data->vertex_buffer);
    free(data->normal_buffer);
    free(data->uv_buffer);
    free(data->joint_buffer);
    free(data->weight_buffer);
    free(data->joints);
    for(i = 0; i < data->animations_count; i++) {
        free(data->animations[i].keyframes_pos);
        free(data->animations[i].keyframes_rot);
    }
    free(data->animations);
    free(data);
    return 0;
}