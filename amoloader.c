#include "amoloader.h"

#include "array.h"
#include <stdint.h>

/* Define the indices of the different arrays during loading */
enum AMO_ARRI {
	AMO_ARR_VTX = 0x0,
	AMO_ARR_TEX = 0x1,
	AMO_ARR_NRM = 0x2,
	AMO_ARR_VJN = 0x3,
	AMO_ARR_VWG = 0x4,
	AMO_ARR_IDX = 0x5,
	AMO_ARR_JNT = 0x6,
	AMO_ARR_ANI = 0x7,
	AMO_ARR_KFR = 0x8,
	AMO_ARR_CMV = 0x9,
	AMO_ARR_CMI = 0xa
};


/*
 * Initialize the arrays depending on the given mask.
 *
 * @arrlst: The arrays to initialize
 * @attr_m: The attribute-mask
 */
AMO_INTERN void amo_init_arrs(struct amo_array *arrlst, uint32_t attr_m)
{
	/* Vertex-array */
	size = 3 * sizeof(float); 
	amo_arr_init(&arrlst[AMO_ARR_VTX], size);

	/* Texture-array */
	size = 2 * sizeof(float);
	amo_arr_init(&arrlst[AMO_ARR_TEX], size);

	/* Normal-vector-array */
	size = 3 * sizeof(float);
	amo_arr_init(&arrlst[AMO_ARR_NRM], size);

	/* Vertex-joint-array */
	size = 4 * sizeof(int);
	amo_arr_init(&arrlst[AMO_ARR_VJN], size);

	/* Vertex-joint-weight-array */
	size = 4 * sizeof(float);
	amo_arr_init(&arrlst[AMO_ARR_VWG], size);

	/* Index-array */
	size = ((attr_m & AMO_M_RIG) ? 5 : 3) * sizeof(unsigned int);
	amo_arr_init(&arrlst[AMO_ARR_IDX], size);

	/* Joint-array */
	size = sizeof(struct amo_joint);
	amo_arr_init(&arrlst[AMO_ARR_JNT], size);

	/* Animation-array */
	size = sizeof(struct amo_anim);
	amo_arr_init(&arrlst[AMO_ARR_ANI], size);

	/* Keyframe-array */
	size = sizeof(struct amo_keyfr);
	amo_arr_init(&arrlst[AMO_ARR_ANI], size);

	/* Collision-mesh-vertex-array*/
	size = 3 * sizeof(float);
	amo_arr_init(&arrlst[AMO_ARR_CMV], size);

	/* Collision-mesh-index-array */
	size = 3 * sizeof(unsigned int);
	amo_arr_init(&arrlst[AMO_ARR_CMI], size);
}

/*
 * Initialize the arrays and read data from the given filedescriptor and write
 * it to the respective arrays. This function will not perform any
 * data-validation or completion.
 *
 * @fd: A filedescriptor to read data from
 * @arrlst: An array to write the data to
 * @name: A buffer to write the model-name to
 * @attr_m: A pointer to write the attribute-mask to
 *
 * Returns: 0 on success or -1 if an error occurred
 */
AMO_INTERN int amo_load_file(FILE *fd, struct amo_array *arrlst, 
		char *name, uint32_t *attr_m)
{
	char buf[64];
	enum amo_format fmt = AMO_FORMAT_NONE;
	uint32_t mask = AMO_M_NONE;
	unsigned short size;

	/*
	 * Find and read the file-header.
	 *
	 * <format-type> <model-name> [<attribute-mask>]
	 */
	while(fscanf(fd, "%s", cmd_buf) != EOF) {
		if(strcmp(cmd_buf, "o") == 0) {
			format = AMO_FORMAT_OBJ;
			break;
		}
		else if(strcmp(cmd_buf, "ao") == 0) {
			format = AMO_FORMAT_AMO;
			break;
		}
	}

	/* Return if no format-type has been found */
	if(format == AMO_FORMAT_NONE)
		return NULL;

	/* Read the model-name */
	fscanf(fd, "%s", name);

	/* Read the data-mask, or use default */
	if(format == AMO_FORMAT_AMO)
		fscanf(fd, "%u", &mask);
	else
		mask = AMO_M_MDL;

	/*
	 * Initialize the arrays.
	 */
	amo_init_arrs(arrlst, mask);


	/* 
	 * Go through every line and read the first word. If it's a valid amo
	 * keyword read the arguments and put them into the right position in
	 * the data struct.
	 */
	while(fscanf(fd, "%s", cmd_buf) != EOF) {
		/* v <x> <y> <z> */
		if(strcmp(cmd_buf, "v") == 0) {
			float vtx[3];

			/* Read the data of the new vertex */
			fscanf(fd, "%f %f %f",
					&vtx[0], &vtx[1], &vtx[2]);

			/* Push into array */
			amo_arr_push(&arrlst[AMO_ARR_VTX], vtx, 1);
		}
		/* vt <x> <y> */
		else if(strcmp(cmd_buf, "vt") == 0) {
			float tex[2];

			/* Read the data of the new uv-coordinate */
			fscanf(fd, "%f %f",
					&tex[0], &tex[1]);

			/* Flip vertical uv-coordinates */
			tex[1] = 1.0 - tex[1];

			/* Push into array */
			amo_arr_push(&arrlst[AMO_ARR_TEX], tex, 1);
		}
		/* vn <x> <y> <z> */
		else if(strcmp(cmd_buf, "vn") == 0) {
			float nrm[3];

			/* Read the data of the new normal-vector */
			fscanf(fd, "%f %f %f",
					&nrm[0], &nrm[1], &nrm[2]);

			/* Push into array */
			amo_arr_push(&arrlst[AMO_ARR_NRM], nrm, 1);
		}
		/* vj <joint_1> <joint_2> <joint_3> <joint_4> */
		else if(strcmp(cmd_buf, "vj") == 0) {
			int vjn[4];

			/* Read the data of the vertex-joint */
			fscanf(fd, "%d %d %d %d", 
					&vjn[0], &vjn[1], &vjn[2], &vjn[3]);

			/* Push into array */
			amo_arr_push(&arrlst[AMO_ARR_VJN], vjn, 1);
		}
		/* vw <weight_1> <weight_2> <weight_3> <weight_4> */
		else if(strcmp(cmd_buf, "vw") == 0) {
			float vwg[4];

			/* Read the data of the new weights */
			fscanf(fd, "%f %f %f %f",
					&vwg[0], &vwg[1], &vwg[2], &vwg[3]);

			/* Push into array */
			amo_arr_push(&arrlst[AMO_ARR_VWG], vwg, 1);
		}
		/* 
		 * f <pos>/<tex>/<nrm>[/<joint>/<weight>]
		 *   <pos>/<tex>/<nrm>[/<joint>/<weight>]
		 *   <pos>/<tex>/<nrm>[/<joint>/<weight>]
		 */
		else if(strcmp(cmd_buf, "f") == 0) {
			unsigned int idx[5];
			int num = (attr_m & AMO_M_RIG) ? 5 : 3;
			int k;

			/* Read the indices in blocks of 3 with 5 each*/
			for(j = 0; j < 3; j++) {
				if(attr_m & AMO_M_RIG) {
					fscanf(fd, "%u/%u/%u/%u/%u",
							&idx[0], &idx[1],
							&idx[2], &idx[3],
							&idx[4]);

				}
				else {
					fscanf(fd, "%u/%u/%u",
							&idx[0], &idx[1],
							&idx[2]);
				}

				/* Decrement by 1, as OBJ indices start at 1 */
				for(k = 0; k < num; k++)
					idx[k] -= 1;
			}

			/* Push into array */
			amo_arr_push(&arrlst[AMO_ARR_IDX], 1);
		}
		/* j <name> <parent> */
		else if(strcmp(cmd_buf, "j") == 0) {
			struct amo_joint jnt;
			int jnt_par;

			/* Read the data of the new joint */
			fscanf(fd, "%s %d",
					jnt.name, &jnt_par);

			/* Read the local base-matrix of this joint */
			for(i = 0; i < 16; i++) {
				fscanf(fd, "%f",
						&jnt.mat[i]);
			}

			/* Correct parent-index if not root-joint */
			if(jnt_par > 0) {
				jnt_par -= 1;
			}

			/* Set index and parent-joint-index */
			jnt.idx = arrlst[AMO_ARR_JNT].num;	
			jnt.par_idx = jnt_par;

			/* Push into the array */
			amo_arr_push(&arrlst[AMO_ARR_JNT], &jnt, 1);
		}
		/* a <name> */
		else if(strcmp(cmd_buf, "a") == 0) {
			struct amo_anim ani;

			/* Initialize the keyframe array */
			amo_arr_init(&ani.keyfr, sizeof(struct amo_keyfr));

			/* Read the data of the animation */
			fscanf(fd, "%s %d",
					ani.name, &ani.dur);

			

			/* Push into the array */
			amo_arr_push(&arrlst[AMO_ARR_ANI], &ani, 1);
		}
		/* k <timestamp> */
		else if(strcmp(cmd_buf, "k") == 0) {
			int num = (arrlst[AMO_ARR_ANI].num - 1);
			int off = num * sizeof(struct amo_anim);
			struct amo_anim *ani = (struct amo_anim *)&arrlst[AMO_ARR_ANI].buf[off];

			struct amo_keyfr keyfr;


			tmp = data->jnt_c * sizeof(struct amo_joint);
			if(!(keyfr.joints = malloc(tmp)))
				goto err_free_data;

			tmp = data->jnt_c * 3 * sizeof(float);
			if(!(keyfr.pos = malloc(tmp))) {
				free(keyfr.joints);
				goto err_free_data;
			}

			tmp = data->jnt_c * 4 * sizeof(float);
			if(!(keyfr.rot = malloc(tmp))) {
				free(keyfr.joints);
				free(keyfr.pos);
				goto err_free_data;
			}


			/* Read the data of the keyframe */
			tmp = ani->keyfr_c - 1;
			fscanf(fd, "%f",
					&keyfr.prog);

			/* Push into the array */
			amo_arr_push(&ani->keyfr, &keyfr, 1);
		}
		/* ap <joint> <x> <y> <z> */
		else if(strcmp(cmd_buf, "ap") == 0) {
			int joint;
			struct amo_anim *ani = &data->ani_lst[data->ani_c - 1];
			struct amo_keyfr *keyfr;

			/* Get a pointer to the keyframe */
			tmp = ani->keyfr_c - 1;
			keyfr = &ani->keyfr_lst[tmp];

			/* Read the data of the position-keyframe */
			fscanf(fd, "%d",
					&joint);

			/* Set a pointer to the referenced joint */
			keyfr->joints[joint - 1] = &data->jnt_lst[joint - 1];

			/* Read the rest of the position-keyframe */
			tmp = (joint - 1) * 3;
			fscanf(fd, "%f %f %f",
					&keyfr->pos[tmp + 0],
					&keyfr->pos[tmp + 1],
					&keyfr->pos[tmp + 2]);
		}
		/* ar <joint> <x> <y> <z> <w> */
		else if(strcmp(cmd_buf, "ar") == 0) {
			int joint;
			struct amo_anim *ani = &data->ani_lst[data->ani_c - 1];
			struct amo_keyfr *keyfr;

			/* Get a pointer to the keyframe */
			tmp = ani->keyfr_c - 1;
			keyfr = &ani->keyfr_lst[tmp];

			/* Read the data of the position-keyframe */
			fscanf(fd, "%d",
					&joint);

			/* Set a pointer to the referenced joint */
			keyfr->joints[joint - 1] = &data->jnt_lst[joint - 1];

			/* Read the rest of the position-keyframe */
			tmp = (joint - 1) * 4;
			fscanf(fd, "%f %f %f %f",
					&keyfr->rot[tmp + 0],
					&keyfr->rot[tmp + 1],
					&keyfr->rot[tmp + 2],
					&keyfr->rot[tmp + 3]);
		}
		/* bp <x> <y> <z> <sx> <sy> <sz> */
		else if(strcmp(cmd_buf, "bp") == 0) {
			fscanf(fd, "%f %f %f %f %f %f",
					&data->bp_col.pos[0],
					&data->bp_col.pos[1],
					&data->bp_col.pos[2],
					&data->bp_col.scl[0],
					&data->bp_col.scl[1],
					&data->bp_col.scl[2]);
		}
		/* ne <x> <y> <z> <sx> <sy> <sz> */
		else if(strcmp(cmd_buf, "ne") == 0) {
			/* Read the data of the near-elipsoid-buffer */
			fscanf(fd, "%f %f %f %f %f %f",
					&data->ne_col.pos[0],
					&data->ne_col.pos[1],
					&data->ne_col.pos[2],
					&data->ne_col.scl[0],
					&data->ne_col.scl[1],
					&data->ne_col.scl[2]);
		}
		/* cv <x> <y> <z> */
		else if(strcmp(cmd_buf, "cv") == 0) {
			/* Increment the number of vertices */
			data->cm_vtx_c++;

			/* Allocate memory if necessary */
			if(data->cm_vtx_c == 1) {
				cm_vtx_num = 200;
				tmp = sizeof(float) * 3 * cm_vtx_num;

				if(!(data->cm_vtx_buf = calloc(1, tmp)))
					goto err_free_data;
			}
			else if(data->cm_vtx_c > cm_vtx_num) {
				cm_vtx_num *= 1.5;
				tmp = sizeof(float) * 3 * cm_vtx_num;

				if(!(p = realloc(data->cm_vtx_buf, tmp)))
					goto err_free_data;

				data->cm_vtx_buf = p;
			}

			/* Read the data of the new vertex */
			tmp = (data->cm_vtx_c - 1) * 3;
			fscanf(fd, "%f %f %f",
					&data->cm_vtx_buf[tmp + 0],
					&data->cm_vtx_buf[tmp + 1],
					&data->cm_vtx_buf[tmp + 2]);
		}
		/* ci <v1> <v2> <v3> */
		else if(strcmp(cmd_buf, "ci") == 0) {
			/* Increment the number of index-blocks */
			data->cm_idx_c++;

			/* Allocate memory if necessary */
			if(data->cm_idx_c == 1) {
				cm_idx_num = 200;
				tmp = sizeof(int) * 3 * cm_idx_num;

				if(!(data->cm_idx_buf = calloc(1, tmp)))
					goto err_free_data;
			}
			else if(data->cm_idx_c > cm_idx_num) {
				cm_idx_num *= 1.5;
				tmp = sizeof(int) * 3 * cm_idx_num;

				if(!(p = realloc(data->cm_idx_buf, tmp)))
					goto err_free_data;

				data->cm_idx_buf = p;
			}

			/* Read the data of the new vertex */
			tmp = (data->cm_idx_c - 1) * 3;
			fscanf(fd, "%d %d %d",
					&data->cm_idx_buf[tmp + 0],
					&data->cm_idx_buf[tmp + 1],
					&data->cm_idx_buf[tmp + 2]);

			data->cm_idx_buf[tmp + 0] -= 1;
			data->cm_idx_buf[tmp + 1] -= 1;
			data->cm_idx_buf[tmp + 2] -= 1;
		}
	}


}

AMO_API struct amo_model *amo_load(const char *pth)
{
	int i;
	int j;
	int tmp;
	void *p;

	FILE *fd;
	struct amo_model *data;

	char cmd_buf[128];
	enum amo_format format = AMO_FORMAT_NONE;
	uint32_t attr_m = AMO_M_NONE;

	struct amo_array arrlst[11];

	/* Check if file is has the right extension */
	if(strcmp(strrchr(pth, '.'), ".amo") == 0)
		;
	else if(strcmp(strrchr(pth, '.'), ".obj") == 0)
		;
	else
		return NULL;

	/* Try to open the file */
	if(!(fd = fopen(pth, "r")))
		return NULL;

	/* 
	 * Allocate memory for the data-struct.
	 * Note that as we use calloc for allocating memory, all bytes will be
	 * set to 0, and therefore we dont have to initialize the attributes.
	 */
	if(!(data = calloc(sizeof(struct amo_model), 1)))
		goto err_close_file;

	
	/* 
	 * Resize the arrays to make them fit their contents.
	 */

	/* vertex-buffer */
	if(data->vtx_c > 0) {
		tmp = data->vtx_c * 3 * sizeof(float);
		if(!(p = realloc(data->vtx_buf, tmp))) goto err_free_data;
		data->vtx_buf = p;
	}

	/* texture-buffer */
	if(data->tex_c > 0) {
		tmp = data->tex_c * 2 * sizeof(float);
		if(!(p = realloc(data->tex_buf, tmp))) goto err_free_data;
		data->tex_buf = p;
	}

	/* normal-buffer */
	if(data->nrm_c > 0) {
		tmp = data->nrm_c * 3 * sizeof(float);
		if(!(p = realloc(data->nrm_buf, tmp))) goto err_free_data;
		data->nrm_buf = p;
	}

	/* vertex-joint-buffer */
	if(data->vjnt_c > 0) {
		tmp = data->vjnt_c * 4 * sizeof(int);
		if(!(p = realloc(data->vjnt_buf, tmp))) goto err_free_data;
		data->vjnt_buf = p;
	}

	/* weight-buffer */
	if(data->wgt_c > 0) {
		tmp = data->wgt_c * 4 * sizeof(float);
		if(!(p = realloc(data->wgt_buf, tmp))) goto err_free_data;
		data->wgt_buf = p;
	}

	/* index-buffer */
	if(data->idx_c > 0) {
		tmp = (attr_m & AMO_M_RIG) ? 5 : 3;
		tmp = data->idx_c * 3 * tmp * sizeof(unsigned int);
		if(!(p = realloc(data->idx_buf, tmp))) goto err_free_data;
		data->idx_buf = p;
	}

	/* joint-list */
	if(data->jnt_c > 0) {
		tmp = data->jnt_c * sizeof(struct amo_joint);
		if(!(p = realloc(data->jnt_lst, tmp))) goto err_free_data;
		data->jnt_lst = p;
	}

	/* animation-list */
	if(data->ani_c > 0) {
		tmp = data->ani_c * sizeof(struct amo_anim);
		if(!(p = realloc(data->ani_lst, tmp))) goto err_free_data;
		data->ani_lst = p;

		for(i = 0; i < data->ani_c; i++) {
			struct amo_anim *ani = &data->ani_lst[i];
	
			tmp = ani->keyfr_c * sizeof(struct amo_keyfr);
			if(!(p = realloc(ani->keyfr_lst, tmp))) goto err_free_data;
			ani->keyfr_lst = p;
		}
	}

	/* collision-vertex-buffer */
	if(data->cm_vtx_c > 0) {
		tmp = data->cm_vtx_c * 3 * sizeof(float);
		if(!(p = realloc(data->cm_vtx_buf, tmp))) goto err_free_data;
		data->cm_vtx_buf = p;
	}

	/* collision-index-buffer */
	if(data->cm_idx_c > 0) {
		tmp = data->cm_idx_c * 3 * sizeof(int);
		if(!(p = realloc(data->cm_idx_buf, tmp))) goto err_free_data;
		data->cm_idx_buf = p;
	}

	/* Close the file */
	fclose(fd);

	/* Return the data */
	return data;

err_free_data:
	/* Free the allocated memory */
	amo_destroy(data);

err_close_file:
	fclose(fd);
	return NULL;
}


AMO_API void amo_destroy(struct amo_model *data)
{
	int i;
	int j;

	/* If data is NULL, just skip destroying */
	if(!data)
		return;

	free(data->vtx_buf);
	free(data->tex_buf);
	free(data->nrm_buf);

	if(data->jnt_c != 0) {
		free(data->vjnt_buf);
		free(data->wgt_buf);
		free(data->jnt_lst);

		for(i = 0; i < data->ani_c; i++) {
			for(j = 0; j < data->ani_lst[i].keyfr_c; j++) {
				free(data->ani_lst[i].keyfr_lst[j].joints);
				free(data->ani_lst[i].keyfr_lst[j].pos);
				free(data->ani_lst[i].keyfr_lst[j].rot);
			}

			free(data->ani_lst[i].keyfr_lst);
		}

		free(data->ani_lst);
	}

	free(data);
}


AMO_API int amo_getdata(struct amo_model *data, int *vtxnum, void **vtx,
		void **tex, void **nrm, void **jnt, void **wgt, int *idxnum,
		unsigned int **idx)
{
	int j;
	int k;
	int tmp;
	char same = 0;
	int num;
	char jnt_flg = 0;

	int vtx_num = 0;
	int vtx_i = 0;

	float *vtx_arr = NULL;
	float *tex_arr = NULL;
	float *nrm_arr = NULL;
	int *jnt_arr = NULL;
	float *wgt_arr = NULL;

	int *idx_conv = NULL;
	unsigned int *idx_arr = NULL;
	int idx_i = 0;
	int idx_bytes = 3;
	int idx_sz = 3 * sizeof(unsigned int);

	/* Return joints only if requested and possible */
	if(jnt != NULL && wgt != NULL && data->jnt_c != 0)
		jnt_flg = 1;

	/*
	 * Count the number of indices and allocate the necessary memory.
	 */

	tmp = data->idx_c * 3;

	if(!(idx_conv = malloc(tmp * sizeof(int))))
		return -1;

	if(!(idx_arr = malloc(tmp * sizeof(unsigned int))))
		goto err_free_arr;

	/*
	 * Bring order to the index-array.
	 */

	/* Get the format of the model */
	idx_bytes = (data->attr_m & AMO_M_RIG) ? 5 : 3;
	idx_sz = 3 * sizeof(unsigned int);

	num = data->idx_c * 3 * idx_bytes;
	for(j = 0; j < num; j += idx_bytes) {
		unsigned int cur[3], cmp[3];

		same = 0;
		memcpy(cur, &data->idx_buf[j], idx_sz);

		for(k = 0; k < idx_i; k++) {
			if(idx_conv[k] < 0)
				continue;
			
			memcpy(cmp, &data->idx_buf[idx_conv[k]], idx_sz);

			if(memcmp(cur, cmp, idx_sz) == 0) {
				same = 1;

				idx_conv[idx_i] = -1;
				idx_arr[idx_i] = idx_arr[k];
				idx_i++;
				break;
			}
		}

		if(!same) {
			idx_conv[idx_i] = j;
			idx_arr[idx_i] = vtx_num;

			idx_i++;
			vtx_num++;
		}
	}

	if(!(vtx_arr = malloc(vtx_num * 3 * sizeof(float))))
		goto err_free_arr;

	if(!(tex_arr = malloc(vtx_num * 2 * sizeof(float))))
		goto err_free_arr;

	if(!(nrm_arr = malloc(vtx_num * 3 * sizeof(float))))
		goto err_free_arr;

	if(jnt_flg) {
		if(!(jnt_arr = malloc(vtx_num * 4 * sizeof(int))))
			goto err_free_arr;

		if(!(wgt_arr = malloc(vtx_num * 4 * sizeof(float))))
			goto err_free_arr;
	}


	/* 
	 * Fill in vertex-data.
	 */
	for(j = 0; j < idx_i; j++) {
		if(idx_conv[j] < 0)
			continue;

		/* Copy vertex-position */
		tmp = data->idx_buf[idx_conv[j]];
		memcpy(&vtx_arr[vtx_i * 3], &data->vtx_buf[tmp * 3],
				3 * sizeof(float));

		/* Copy uv-coordinate */
		tmp = data->idx_buf[idx_conv[j] + 1];
		memcpy(&tex_arr[vtx_i * 2], &data->tex_buf[tmp * 2],
				2 * sizeof(float));

		/* Copy normal-vector */
		tmp = data->idx_buf[idx_conv[j] + 2];
		memcpy(&nrm_arr[vtx_i * 3], &data->nrm_buf[tmp * 3],
				3 * sizeof(float));

		if(jnt_arr) {
			/* Copy joint-indices */
			tmp = data->idx_buf[idx_conv[j] + 3];
			memcpy(&jnt_arr[vtx_i * 4], &data->vjnt_buf[tmp * 4],
				4 * sizeof(int));

			/* Copy joint-weights */
			tmp = data->idx_buf[idx_conv[j] + 4];
			memcpy(&wgt_arr[vtx_i * 4], &data->wgt_buf[tmp * 4],
				4 * sizeof(float));
		}

		vtx_i++;
	}

	/*
	 * Free the index-convert-array.
	 */
	free(idx_conv);

	/*
	 * Return all arrays.
	 */
	*vtxnum = vtx_i;
	*vtx = vtx_arr;
	*tex = tex_arr;
	*nrm = nrm_arr;
	*jnt = jnt_arr;
	*wgt = wgt_arr;

	*idxnum = idx_i;
	*idx = idx_arr;
	return 0;

err_free_arr:
	if(vtx_arr) free(vtx_arr);
	if(tex_arr) free(tex_arr);
	if(nrm_arr) free(nrm_arr);
	if(jnt_arr) free(jnt_arr);
	if(wgt_arr) free(wgt_arr);
	if(idx_conv) free(idx_conv);
	if(idx_arr) free(idx_arr);
	return -1;
}
