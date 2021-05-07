#include "amoloader.h"

#include "array.h"
#include <stdint.h>


AMO_API struct amo_model *amo_load(FILE *fd)
{
	int i;
	int j;
	int tmp;

	struct amo_model *data;

	char opbuf[48];
	char check = 0;

	int ani_itr = -1;
	int keyfr_itr = -1;

	/* Verify the passed filedescriptor is valid */
	if(fd == NULL)
		return NULL;

	/* 
	 * Allocate memory for the data-struct.
	 * Note that as we use calloc for allocating memory, all bytes will be
	 * set to 0, and therefore we dont have to initialize the attributes.
	 */
	if(!(data = calloc(sizeof(struct amo_model), 1)))
		goto err_close_file;

	/* Reset the buffers */
	data->vtx_buf = NULL;
	data->tex_buf = NULL;
	data->nrm_buf = NULL;
	data->vjnt_buf = NULL;
	data->wgt_buf = NULL;
	data->idx_buf = NULL;
	data->jnt_lst = NULL;
	data->ani_lst = NULL;
	data->hk_lst = NULL;
	data->cm_vtx_buf = NULL;
	data->cm_idx_buf = NULL;
	data->cm_nrm_buf = NULL;
	data->rb_jnt = NULL;
	data->rb_pos = NULL;
	data->rb_scl = NULL;
	data->rb_mat = NULL;

	/*
	 * Search for the format-type.
	 *
	 * <format-type> <model-name>
	 */
	while(fscanf(fd, "%s", opbuf) != EOF) {
		if(strcmp(opbuf, "ao") == 0) {
			check = 1;
			break;
		}
	}

	/*
	 * If no format-type has been found, then terminate.
	 */
	if(check == 0)
		return NULL;

	/*
	 * Read the model-name.
	 */
	fscanf(fd, "%s", data->name);

	/* Read the data-mask */
	fscanf(fd, "%u", &data->attr_m);

	/* 
	 * Go through every line and read the first word. If it's a valid amo
	 * keyword read the arguments and put them into the right position in
	 * the data struct.
	 */
	while(fscanf(fd, "%s", opbuf) != EOF) {
		if(strcmp(opbuf, "end") == 0)
			break;

		/* v <x> <y> <z> */
		if(strcmp(opbuf, "v") == 0) {
			/* Read the number of vertices */
			fscanf(fd, "%d", &data->vtx_c);

			/* Allocate memory */
			tmp = sizeof(float) * 3 * data->vtx_c;
			if(!(data->vtx_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the vertices */
			for(i = 0; i < data->vtx_c; i++) {
				tmp = i * 3;
				fscanf(fd, "%f %f %f",
						&data->vtx_buf[tmp + 0],
						&data->vtx_buf[tmp + 1],
						&data->vtx_buf[tmp + 2]);
			}
		}
		/* vt <x> <y> */
		else if(strcmp(opbuf, "vt") == 0) {
			/* Read the number of texture-coordinates */
			fscanf(fd, "%d", &data->tex_c);
			
			/* Allocate memory */
			tmp = sizeof(float) * 2 * data->tex_c;
			if(!(data->tex_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the new uv-coordinate */
			for(i = 0; i < data->tex_c; i++) {
				tmp = i * 2;
				fscanf(fd, "%f %f",
						&data->tex_buf[tmp + 0],
						&data->tex_buf[tmp + 1]);

				/* Flip uv-coordinates */
				data->tex_buf[tmp + 1] = 1.0 -
					data->tex_buf[tmp + 1];
			}
		}
		/* vn <x> <y> <z> */
		else if(strcmp(opbuf, "vn") == 0) {
			/* Increment the number of normal-vectors */
			fscanf(fd, "%d", &data->nrm_c);

			/* Allocate memory */
			tmp = sizeof(float) * 3 * data->nrm_c;
			if(!(data->nrm_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the new normal-vectors */
			for(i = 0; i < data->nrm_c; i++) {
				tmp = i * 3;
				fscanf(fd, "%f %f %f",
						&data->nrm_buf[tmp + 0],
						&data->nrm_buf[tmp + 1],
						&data->nrm_buf[tmp + 2]);
			}
		}
		/* vj <joint_1> <joint_2> <joint_3> <joint_4> */
		else if(strcmp(opbuf, "vj") == 0) {
			/* Read the number of vertex-joints */
			fscanf(fd, "%d", &data->vjnt_c);

			/* Allocate memory */
			tmp = sizeof(int) * 4 * data->vjnt_c;
			if(!(data->vjnt_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the vertex-joints */
			for(i = 0; i < data->vjnt_c; i++) {
				tmp = i * 4;
				fscanf(fd, "%d %d %d %d",
						&data->vjnt_buf[tmp + 0],
						&data->vjnt_buf[tmp + 1],
						&data->vjnt_buf[tmp + 2],
						&data->vjnt_buf[tmp + 3]);
			}
		}
		/* vw <weight_1> <weight_2> <weight_3> <weight_4> */
		else if(strcmp(opbuf, "vw") == 0) {
			/* Read the number of joints */
			fscanf(fd, "%d", &data->wgt_c);
				
			/* Allocate memory */
			tmp = sizeof(float) * 4 * data->wgt_c;
			if(!(data->wgt_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the new weights */
			for(i = 0; i < data->wgt_c; i++) {
				tmp = i * 4;
				fscanf(fd, "%f %f %f %f",
						&data->wgt_buf[tmp + 0],
						&data->wgt_buf[tmp + 1],
						&data->wgt_buf[tmp + 2],
						&data->wgt_buf[tmp + 3]);
			}
		}
		/* 
		 * f <pos>/<tex>/<normal>/<joint>/<weight>
		 *   <pos>/<tex>/<normal>/<joint>/<weight>
		 *   <pos>/<tex>/<normal>/<joint>/<weight>
		 */
		else if(strcmp(opbuf, "f") == 0) {
			int num = (data->attr_m & AMO_M_RIG) ? 5 : 3;

			/* Read the number of index-blocks */
			fscanf(fd, "%d", &data->idx_c);

			/* Allocate memory */
			tmp = sizeof(unsigned int) * (3 * num) * data->idx_c;
			if(!(data->idx_buf = calloc(1, tmp)))
				goto err_free_data;

			/* Read the indices in blocks of 3 with 5 each */
			for(i = 0; i < data->idx_c; i++) {
				for(j = 0; j < 3; j++) {
					if(data->attr_m & AMO_M_RIG) {
						tmp = (i * (3 * num)) + (j * num);
						fscanf(fd, "%u %u %u %u %u",
								&data->idx_buf[tmp + 0],
								&data->idx_buf[tmp + 1],
								&data->idx_buf[tmp + 2],
								&data->idx_buf[tmp + 3],
								&data->idx_buf[tmp + 4]);
					}
					else {
						tmp = (i * (3 * num)) + (j * num);
						fscanf(fd, "%u %u %u",
								&data->idx_buf[tmp + 0],
								&data->idx_buf[tmp + 1],
								&data->idx_buf[tmp + 2]);
					}
				}
			}
		}
		/* 
		 * j <joint-num> 
		 * <name> <parent> <matrix>
		 */
		else if(strcmp(opbuf, "j") == 0) {
			int joint_par;
			struct amo_joint *joint_tmp;

			/* Read the number of joints */
			fscanf(fd, "%d", &data->jnt_c);

			/* Allocate memory */
			tmp = sizeof(struct amo_joint) * data->jnt_c;
			if(!(data->jnt_lst = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the joints */
			for(i = 0; i < data->jnt_c; i++) {
				fscanf(fd, "%s %d",
					data->jnt_lst[i].name,
					&joint_par);

				/* Read the rest-bone-matrix */
				for(j = 0; j < 16; j++)
					fscanf(fd, "%f",
							&data->jnt_lst[i].mat[j]);

				/* Set the index and parent-joint */
				data->jnt_lst[i].index = i;
				data->jnt_lst[i].par = joint_par;
			}
		}
		/* 
		 * a <anim-num>
		 * ...
		 */
		else if(strcmp(opbuf, "a") == 0) {
			/* Read the number of animations */
			fscanf(fd, "%d", &data->ani_c);

			/* Allocate memory */
			tmp = sizeof(struct amo_anim) * data->ani_c;
			if(!(data->ani_lst = malloc(tmp)))
				goto err_free_data;

			ani_itr = -1;
		}
		/*
		 * an <name> <duration> <keyfr-num>
		 * ...
		 */
		else if(strcmp(opbuf, "an") == 0) {
			int tmp_num;

			/* Increment the animation-iterator */
			ani_itr++;

			/* Read both the name and duration of the animation,
			 * plus the number of keyframes */
			fscanf(fd, "%s", data->ani_lst[ani_itr].name);
			fscanf(fd, "%d", &data->ani_lst[ani_itr].dur);
			fscanf(fd, "%d", &tmp_num);
			data->ani_lst[ani_itr].keyfr_c = tmp_num;

			/* Allocate memory for keyframes */
			tmp = sizeof(struct amo_keyfr) * tmp_num;
			if(!(data->ani_lst[ani_itr].keyfr_lst = malloc(tmp)))
				goto err_free_data;

			for(i = 0; i < tmp_num; i++) {
				data->ani_lst[ani_itr].keyfr_lst[i].jnt = NULL;
				data->ani_lst[ani_itr].keyfr_lst[i].pos = NULL;
				data->ani_lst[ani_itr].keyfr_lst[i].rot = NULL;
			}

			keyfr_itr = -1;
		}
		/*
		 * k <progress> <jnt-num>
		 * <jnt-idx> <px> <py> <pz> <rw> <rx> <ry> <rz>
		 */
		else if(strcmp(opbuf, "k") == 0) {
			struct amo_anim *ani;
			struct amo_keyfr *keyfr;

			/* Increment the keyframe-iterator */
			keyfr_itr++;


			/* Get a pointer to both the animation-struct and
			 * keyframe-struct */
			ani = &data->ani_lst[ani_itr];

			keyfr = &ani->keyfr_lst[keyfr_itr];

			/* Read the progress and the joint-number */
			fscanf(fd, "%f", &keyfr->prog);
			fscanf(fd, "%d", &keyfr->jnt_num);

			/* Allocate memory for data */	
			tmp = keyfr->jnt_num * sizeof(short);
			if(!(keyfr->jnt = malloc(tmp)))
				goto err_free_data;	

			tmp = keyfr->jnt_num * 3 * sizeof(float);
			if(!(keyfr->pos = malloc(tmp)))
				goto err_free_data;

			tmp = keyfr->jnt_num * 4 * sizeof(float);
			if(!(keyfr->rot = malloc(tmp)))
				goto err_free_data;

			/* Read the keyframe-joint-data */
			for(i = 0; i < keyfr->jnt_num; i++) {
				/* Read the joint-index */
				fscanf(fd, "%hd", &keyfr->jnt[i]);

				/* Read the position */
				fscanf(fd, "%f %f %f",
						&keyfr->pos[i * 3 + 0],
						&keyfr->pos[i * 3 + 1],
						&keyfr->pos[i * 3 + 2]);

				/* Read the rotation */
				fscanf(fd, "%f %f %f %f",
						&keyfr->rot[i * 4 + 0],
						&keyfr->rot[i * 4 + 1],
						&keyfr->rot[i * 4 + 2],
						&keyfr->rot[i * 4 + 3]);
			}
		}
		/* 
		 * hk <hook-num>
		 * <idx> <par_jnt> <pos>
		 */
		else if(strcmp(opbuf, "hk") == 0) {
			/* Read the number of hooks */
			fscanf(fd, "%d", &data->hk_c);

			/* Allocate memory */
			tmp = sizeof(struct amo_hook) * data->hk_c;
			if(!(data->hk_lst = malloc(tmp)))
				goto err_free_data;

			/* Read the data */
			for(i = 0; i < data->hk_c; i++) {
				/* Read the index */
				fscanf(fd, "%hd",
						&data->hk_lst[i].idx);

				/* Read the parent-joint */
				fscanf(fd, "%hd",
						&data->hk_lst[i].par_jnt);


				/* Read the hook-position */
				fscanf(fd, "%f %f %f",
						&data->hk_lst[i].pos[0],
						&data->hk_lst[i].pos[1],
						&data->hk_lst[i].pos[2]);

				/* Read the forward-vector */
				fscanf(fd, "%f %f %f",
						&data->hk_lst[i].dir[0],
						&data->hk_lst[i].dir[1],
						&data->hk_lst[i].dir[2]);

				/* Read the local matrix */
				for(j = 0; j < 16; j++)
					fscanf(fd, "%f",
							&data->hk_lst[i].mat[j]);
			}
		}
		/* bp <x> <y> <z> <sx> <sy> <sz> */
		else if(strcmp(opbuf, "bb") == 0) {
			fscanf(fd, "%f %f %f %f %f %f",
					&data->bb_col.pos[0],
					&data->bb_col.pos[1],
					&data->bb_col.pos[2],
					&data->bb_col.scl[0],
					&data->bb_col.scl[1],
					&data->bb_col.scl[2]);
		}
		/* ne <x> <y> <z> <sx> <sy> <sz> */
		else if(strcmp(opbuf, "ne") == 0) {
			/* Read the data of the near-elipsoid-buffer */
			fscanf(fd, "%f %f %f %f %f %f",
					&data->ne_col.pos[0],
					&data->ne_col.pos[1],
					&data->ne_col.pos[2],
					&data->ne_col.scl[0],
					&data->ne_col.scl[1],
					&data->ne_col.scl[2]);
		}
		/* 
		 * cv <vertex-num>
		 * <x> <y> <z>
		 */
		else if(strcmp(opbuf, "cv") == 0) {
			/* Read the number of collision-mesh-vertices */
			fscanf(fd, "%d", &data->cm_vtx_c);

			/* Allocate memory */
			tmp = sizeof(float) * 3 * data->cm_vtx_c;
			if(!(data->cm_vtx_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the vertices */
			for(i = 0; i < data->cm_vtx_c; i++) {
				tmp = i * 3;
				fscanf(fd, "%f %f %f",
					&data->cm_vtx_buf[tmp + 0],
					&data->cm_vtx_buf[tmp + 1],
					&data->cm_vtx_buf[tmp + 2]);
			}
		}
		/* 
		 * ci <index-block-num> 
		 * <v1> <v2> <v3>
		 */
		else if(strcmp(opbuf, "ci") == 0) {
			/* Read the number of index-blocks */
			fscanf(fd, "%d", &data->cm_idx_c);

			/* Allocate memory */
			tmp = sizeof(int) * 3 * data->cm_idx_c;
			if(!(data->cm_idx_buf = malloc(tmp)))
				goto err_free_data;

			/* Read the data of the vertices */
			for(i = 0; i < data->cm_idx_c; i++) {
				tmp = i * 3;
				fscanf(fd, "%d %d %d",
						&data->cm_idx_buf[tmp + 0],
						&data->cm_idx_buf[tmp + 1],
						&data->cm_idx_buf[tmp + 2]);
			}
		}
		/* 
		 * rb <box-num>
		 * <jnt> <pos> <scl> <mat>
		 */
		else if(strcmp(opbuf, "rb") == 0) {
			/* Read the number of rig-boxes */
			fscanf(fd, "%d", &data->rb_c);

			/* Allocate memory */	
			tmp = sizeof(int) * data->rb_c;
			if(!(data->rb_jnt = malloc(tmp)))
				goto err_free_data;

			tmp = sizeof(float) * 3 * data->rb_c;
			if(!(data->rb_pos = malloc(tmp)))
				goto err_free_data;

			tmp = sizeof(float) * 3 * data->rb_c;
			if(!(data->rb_scl = malloc(tmp)))
				goto err_free_data;

			tmp = sizeof(float) * 16 * data->rb_c;
			if(!(data->rb_mat = malloc(tmp)))
				goto err_free_data;

			for(i = 0; i < data->rb_c; i++) {
				/* Read the parent-joint-index */
				fscanf(fd, "%d",
						&data->rb_jnt[i]);

				/* Read the position of the rig-box */
				tmp = i * 3;
				fscanf(fd, "%f %f %f",
						&data->rb_pos[tmp + 0],
						&data->rb_pos[tmp + 1],
						&data->rb_pos[tmp + 2]);

				/* Read the scale of the rig-box */
				tmp = i * 3;
				fscanf(fd, "%f %f %f",
						&data->rb_scl[tmp + 0],
						&data->rb_scl[tmp + 1],
						&data->rb_scl[tmp + 2]);

				/* Read the transformation-matrix of the box */
				tmp = i * 16;
				for(i = 0; i < 16; i++) {
					fscanf(fd, "%f",
							&data->rb_mat[tmp + i]);
				}
			}
		}
	}

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
	struct amo_anim *ani;
	struct amo_keyfr *keyfr;

	/* If data is NULL, just skip destroying */
	if(!data) return;

	if(data->vtx_buf) free(data->vtx_buf);
	if(data->tex_buf) free(data->tex_buf);
	if(data->nrm_buf) free(data->nrm_buf);

	if(data->vjnt_buf) free(data->vjnt_buf);
	if(data->wgt_buf) free(data->wgt_buf);
	if(data->jnt_lst) free(data->jnt_lst);

	for(i = 0; i < data->ani_c; i++) {
		ani = &data->ani_lst[i];

		for(j = 0; j < ani->keyfr_c; j++) {
			keyfr = &ani->keyfr_lst[j];

			if(keyfr->jnt) free(keyfr->jnt);
			if(keyfr->pos) free(keyfr->pos);
			if(keyfr->rot) free(keyfr->rot);
		}

		if(ani->keyfr_lst) free(ani->keyfr_lst);
	}

	if(data->ani_lst) free(data->ani_lst);

	if(data->cm_vtx_buf) free(data->cm_vtx_buf);
	if(data->cm_idx_buf) free(data->cm_idx_buf);
	if(data->cm_nrm_buf) free(data->cm_nrm_buf);

	if(data->rb_jnt) free(data->rb_jnt);
	if(data->rb_pos) free(data->rb_pos);
	if(data->rb_scl) free(data->rb_scl);
	if(data->rb_mat) free(data->rb_mat);

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
