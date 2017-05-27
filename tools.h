#define hashtable_size  10000
typedef struct barcode_list_element{
	unsigned long ten_x_barcode;
	unsigned long count;
	struct barcode_list_element* next;
}barcode_list_element;

struct barcode_list_element* hashtable[hashtable_size];

//Takes a string representing a barcode tag and encodes it as an unsigned long.
//An empty string will be encoded as (unsigned long) -1. Since barcodes are 16 
//nucleotides long, they are encoded to the 32 rightmost bits.
unsigned long encode_ten_x_barcode(char* source);

//Initializes hashtable array for use in barcode_homogeneity_score()
void init_barcode_homogeneity_hashtable();

//Frees hashtable array after use in barcode_homogeneity_score()
void free_barcode_homogeneity_hashtable();

//Takes an int ID for an element from listClusterEl[] list. This list and its 
//elements are defined in TARDIS. The functions then takes the reads in this 
//cluster and reads their ten_x_barcode. It computes the homogeneity score of
//this cluster, discarding any invalid barcodes (equal to -1) and returns the
//score.
double barcode_homogeneity_score(int clusterId);

//Takes a string that contains qname extracted from a BAM file and appends 
//ten_x_barcode as a string to it. qname should have enough space to accomodate
//20 more characters after its NULL terminator.
//This function is usefull for storing extracted reads from BAM files to FASTQ
//format
void append_ten_x_barcode_to_qname(char* qname, unsigned long ten_x_barcode);

//This extracts ten_x_barcode from a string that have had ten_x_barcode appended
//to it using append_ten_x_barcode_to_qname() function
unsigned long extract_ten_x_barcode_from_qname(char* qname);

//Takes a library ID and adds it to ten_x_barcode. This avoids the confusion
//that arises from having two identical barcodes coming from two different 
//sequencing libraries. The library ID is assumed to be less than or equal to
//255. The library ID will be stored to ten_x_barcode leftmost byte. Invalid 
//barcodes (equal to -1) should NOT be passed to this function.
unsigned long mask_library_id_to_ten_x_barcode(unsigned long lib_id, unsigned long ten_x_barcode);
