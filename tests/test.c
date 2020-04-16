#include <amoloader.h>

int main() {
    int i, j;
    struct amoloader_data *data = load_amo("tests/cube.amo");
    if(data == NULL) {
        fprintf(stderr, "Couldn't load amo file\n");
        return -1;
    }
    for(i = 0; i < data->vertices_count; i++) {
        printf("pos: %f %f %f normal: %f %f %f uv: %f %f joints: %d %d %d %d weights: %f %f %f %f\n", data->vertex_buffer[i*3],
                                                                                                      data->vertex_buffer[i*3+1],
                                                                                                      data->vertex_buffer[i*3+2],
                                                                                                      data->normal_buffer[i*3],
                                                                                                      data->normal_buffer[i*3+1],
                                                                                                      data->normal_buffer[i*3+2],
                                                                                                      data->uv_buffer[i*2],
                                                                                                      data->uv_buffer[i*2+1],
                                                                                                      data->joint_buffer[i*4],
                                                                                                      data->joint_buffer[i*4+1],
                                                                                                      data->joint_buffer[i*4+2],
                                                                                                      data->joint_buffer[i*4+3],
                                                                                                      data->weight_buffer[i*4],
                                                                                                      data->weight_buffer[i*4+1],
                                                                                                      data->weight_buffer[i*4+2],
                                                                                                      data->weight_buffer[i*4+3]);
    }
    for(i = 0; i < data->indices_count; i++) {
        printf("index: %u %u %u\n", data->index_buffer[i*3],
                                    data->index_buffer[i*3+1],
                                    data->index_buffer[i*3+2]);
    }
    for(i = 0; i < data->joints_count; i++) {
        printf("joint: %s parent: %p\n", data->joints[i].name,
                                         (void*)data->joints[i].parent);
    }
    for(i = 0; i < data->animations_count; i++) {
        printf("animation: %s\n", data->animations[i].name);
        for(j = 0; j < data->animations[i].keyframes_pos_count; j++) {
            printf("animated pos: time: %f joint: %s pos: %f %f %f\n", data->animations[i].keyframes_pos[j].timestamp,
                                                                       data->animations[i].keyframes_pos[j].joint->name,
                                                                       data->animations[i].keyframes_pos[j].pos[0],
                                                                       data->animations[i].keyframes_pos[j].pos[1],
                                                                       data->animations[i].keyframes_pos[j].pos[2]);
        }
        for(j = 0; j < data->animations[i].keyframes_rot_count; j++) {
            printf("animated rot: time %f joint: %s rot: %f %f %f %f\n", data->animations[i].keyframes_rot[j].timestamp,
                                                                         data->animations[i].keyframes_rot[j].joint->name,
                                                                         data->animations[i].keyframes_rot[j].rot[0],
                                                                         data->animations[i].keyframes_rot[j].rot[1],
                                                                         data->animations[i].keyframes_rot[j].rot[2],
                                                                         data->animations[i].keyframes_rot[j].rot[3]);
        }
    }
    destroy_amo(data);
    return 0;
}
