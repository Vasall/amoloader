#include <amoloader.h>

int main() {
    int i, j, count;
    struct amoloader_data *data = amo_load("tests/cube.amo", &count);
    if(data == NULL) {
        fprintf(stderr, "Couldn't load amo file\n");
        return -1;
    }
    for(i = 0; i < data[0].vertices_count; i++) {
        printf("pos: %f %f %f normal: %f %f %f uv: %f %f joints: %d %d %d %d weights: %f %f %f %f\n", data[0].vertex_buffer[i*3],
                                                                                                      data[0].vertex_buffer[i*3+1],
                                                                                                      data[0].vertex_buffer[i*3+2],
                                                                                                      data[0].normal_buffer[i*3],
                                                                                                      data[0].normal_buffer[i*3+1],
                                                                                                      data[0].normal_buffer[i*3+2],
                                                                                                      data[0].uv_buffer[i*2],
                                                                                                      data[0].uv_buffer[i*2+1],
                                                                                                      data[0].joint_buffer[i*4],
                                                                                                      data[0].joint_buffer[i*4+1],
                                                                                                      data[0].joint_buffer[i*4+2],
                                                                                                      data[0].joint_buffer[i*4+3],
                                                                                                      data[0].weight_buffer[i*4],
                                                                                                      data[0].weight_buffer[i*4+1],
                                                                                                      data[0].weight_buffer[i*4+2],
                                                                                                      data[0].weight_buffer[i*4+3]);
    }
    for(i = 0; i < data[0].indices_count; i++) {
        printf("index: %u %u %u\n", data[0].index_buffer[i*3],
                                    data[0].index_buffer[i*3+1],
                                    data[0].index_buffer[i*3+2]);
    }
    for(i = 0; i < data[0].joints_count; i++) {
        printf("joint: %s parent: %p\n", data[0].joints[i].name,
                                         (void*)data[0].joints[i].parent);
    }
    for(i = 0; i < data[0].animations_count; i++) {
        printf("animation: %s\n", data[0].animations[i].name);
        for(j = 0; j < data[0].animations[i].keyframes_pos_num; j++) {
            printf("animated pos: time: %f joint: %s pos: %f %f %f\n", data[0].animations[i].keyframes_pos[j].timestamp,
                                                                       data[0].animations[i].keyframes_pos[j].joint->name,
                                                                       data[0].animations[i].keyframes_pos[j].pos[0],
                                                                       data[0].animations[i].keyframes_pos[j].pos[1],
                                                                       data[0].animations[i].keyframes_pos[j].pos[2]);
        }
        for(j = 0; j < data[0].animations[i].keyframes_rot_num; j++) {
            printf("animated rot: time %f joint: %s rot: %f %f %f %f\n", data[0].animations[i].keyframes_rot[j].timestamp,
                                                                         data[0].animations[i].keyframes_rot[j].joint->name,
                                                                         data[0].animations[i].keyframes_rot[j].rot[0],
                                                                         data[0].animations[i].keyframes_rot[j].rot[1],
                                                                         data[0].animations[i].keyframes_rot[j].rot[2],
                                                                         data[0].animations[i].keyframes_rot[j].rot[3]);
        }
    }
    amo_destroy(data, count);
    return 0;
}
