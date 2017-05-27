void init_barcode_homogeneity_hashtable(){
	int hashtable_index;
	//initialize the heads of the chains in the hashtable
	for (hashtable_index = 0; hashtable_index < hashtable_size; hashtable_index++){
		hashtable[hashtable_index] = (barcode_list_element*) malloc(sizeof(barcode_list_element));
		hashtable[hashtable_index]->ten_x_barcode = 0;
		hashtable[hashtable_index]->count = 0;
		hashtable[hashtable_index]->next = NULL;
	}
}

void free_barcode_homogeneity_hashtable(){
	int i;
	struct barcode_list_element *temp_ptr;
	for (i = 0; i < hashtable_size; i++){
		while(hashtable[i]->next){
			temp_ptr = hashtable[i]->next->next;
			free(hashtable[i]->next);
			hashtable[i]->next = temp_ptr;
		}

		free(hashtable[i]);
	}
}

double barcode_homogeneity_score(int clusterId){

	unsigned long total_read_count = 0;
	double sum = 0.0;
	double score = 1.0;
	int i;
	int hashtable_index;
	struct barcode_list_element *current_list_element;
	struct barcode_list_element *temp_ptr;
	readMappingEl *ptrReadMapping;
	int found;

	ptrReadMapping = listClusterEl[clusterId].next;
	while( ptrReadMapping != NULL){
		if( listReadEl[ptrReadMapping->readId].readCovered == 0
				&& listClusterEl[clusterId].indIdCount[ptrReadMapping->indId] > -1
				&& (signed long)ptrReadMapping->ten_x_barcode != -1){

			total_read_count++;

			hashtable_index = ptrReadMapping->ten_x_barcode % hashtable_size;
			current_list_element = hashtable[hashtable_index];

			found = 0;
			while (current_list_element->next){
				if (current_list_element->next->ten_x_barcode == ptrReadMapping->ten_x_barcode){
					current_list_element->next->count++;
					found = 1;
					break;
				}
				current_list_element = current_list_element->next;
			}

			if (found == 0){
				current_list_element->next = (barcode_list_element*)malloc(sizeof(barcode_list_element));
				current_list_element->next->ten_x_barcode = ptrReadMapping->ten_x_barcode;
				current_list_element->next->count = 1;
				current_list_element->next->next = NULL;
			}
		}
		ptrReadMapping=ptrReadMapping->next;
	}

	for (hashtable_index = 0; hashtable_index < hashtable_size; hashtable_index++){
		current_list_element = hashtable[hashtable_index];

		while(current_list_element->next){
			sum = sum + ((double)current_list_element->next->count*(double)current_list_element->next->count);
			current_list_element = current_list_element->next;
		}
	}

	for (i = 0; i < hashtable_size; i++){
		while(hashtable[i]->next){
			temp_ptr = hashtable[i]->next->next;
			free(hashtable[i]->next);
			hashtable[i]->next = temp_ptr;
		}
	}

	if (total_read_count == 0){
		listClusterEl[clusterId].homogeneity_score = score;
		return 1.0;
	}


	score = sum/(total_read_count*total_read_count);

	listClusterEl[clusterId].homogeneity_score = score;

	if (ten_x_flag != 1){
		return 1.0;
	}

	if (!(score > 1 || score < 0))
		return score;
	printf("Err: score isn't between 0-1: %f\n", score);

	return inf;
}

unsigned long encode_ten_x_barcode(char* source){
	int i, len;
	unsigned long next_digit, result;

	if (source == NULL){
		return -1;
	}

	result = 0;
	next_digit = 0;
	len = strlen(source);
	for (i = 0; i < len; i++){
		switch(source[i]){
		case 'A':
			next_digit = 0;
			result = (result << 2)|next_digit;
			break;
		case 'C':
			next_digit = 1;
			result = (result << 2)|next_digit;
			break;
		case 'G' :
			next_digit = 2;
			result = (result << 2)|next_digit;
			break;
		case 'T':
			next_digit = 3;
			result = (result << 2)|next_digit;
			break;
		default:
			break;
		}
	}
	return result;
}

void append_ten_x_barcode_to_qname(char* qname, unsigned long ten_x_barcode){
    sprintf(qname + strlen(qname), "%020lu", ten_x_barcode);
}

unsigned long extract_ten_x_barcode_from_qname(char* qname){
    unsigned long ten_x_barcode;
    sscanf(readName + strlen(readName) - 20, "%020lu%0", &ten_x_barcode);
    return ten_x_barcode;
}

unsigned long mask_library_id_to_ten_x_barcode(unsigned long lib_id, unsigned long ten_x_barcode){
    return ten_x_barcode | (libId << (sizeof(unsigned long)-1)*8);
}
