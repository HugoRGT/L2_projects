//TURAN Baturay TP3B && ROUGETET Hugo TP2B
#include "classes.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"

	

struct tab{
	char **args;
	size_t size;
};



//REQUESTED [DONE]
void hierarchy_create(struct oo_hierarchy *self) {
  struct oo_class *obj_class = malloc(sizeof(struct oo_class));
  //Set size/capacity/name
  obj_class->capacity = 10;
  obj_class->size=0;
  obj_class->name = malloc(8*sizeof(char));
  strcpy(obj_class->name,"Object");
  //Set parent class/children class/root class
  obj_class->parent = NULL;
  obj_class->children = calloc(obj_class->capacity,sizeof(struct oo_class**));

  self->root = obj_class;

}



//[DONE]
void destroy_class(struct oo_class *self){
  if (self == NULL){
    return; //Nothing to destroy, return to the calling method
  }
  
  if (self->size>0){
    //printf("freeing %s's children\n",self->name);
    for (size_t i = 0; i<self->size; i++){
      destroy_class(self->children[i]);
    }
    
  }
  free(self->children);
  //printf("freeing %s\n",self->name);
  free(self->name);
  free(self);
}

//REQUESTED [DONE]
void hierarchy_destroy(struct oo_hierarchy *self) {
  destroy_class(self->root);
}



//[DONE]
size_t class_count_children(struct oo_class *self){
  if (self->size == 0){
    return 0;
  }
  size_t n = 0;
  for (size_t i = 0; i<self->size;i++){
    n=n+class_count_children(self->children[i])+1;
  }
  return n;
}

//REQUESTED[DONE]
size_t hierarchy_count_classes(const struct oo_hierarchy *self) {
  return 1+class_count_children(self->root);
}



//[DONE]
bool class_contains_class(const struct oo_class *self,const char *name){
  if (strcmp(self->name,name)==0){
    return true;
  }
  
  if (self->children==NULL){
    return false;
  }
  
  for(size_t i = 0; i<self->size; i++){
    if (class_contains_class(self->children[i],name)){
      return true;
    }
  }
  
  return false;
}

//REQUESTED [DONE]
bool hierarchy_has_class(const struct oo_hierarchy *self, const char *name) {
  return class_contains_class(self->root,name);
}



//[DONE]
void class_children_swap(struct oo_class *self, int a , int b){
  if (a>=(int)self->size || b>=(int)self->size){return;}
  
  struct oo_class *tmp = self->children[a];
  self->children[a]=self->children[b];
  self->children[b]=tmp;
}

//[DONE]
void class_children_quicksort_recursive(struct oo_class *self , int start, int stop){
  if (self->size <2){return;}
  
  int l = start-1;
  int r = stop+1;
  struct oo_class *pivot = self->children[start];
  
  if (start >= stop){return;}
  while(1){
    do{r--;}while(strcmp(self->children[r]->name,pivot->name)>0);// function(self,pivot)>0
    do{l++;}while(strcmp(self->children[l]->name,pivot->name)<0);
	 
    if (l<r){
      class_children_swap(self,l,r);
    }else{
      break;
    }
  }
  class_children_quicksort_recursive(self,start,r);
  class_children_quicksort_recursive(self,r+1,stop);
}

//[DONE]
void class_children_sort(struct oo_class *self){
  class_children_quicksort_recursive(self,0,self->size-1);
}

//[DONE]
bool class_children_is_sorted(struct oo_class *self){
  for(size_t i = 0; i<self->size-1;i++){
    if (strcmp(self->children[i]->name,self->children[i+1]->name)>0){
      return false;
    }

  }
  return true;
}



//[DONE]
//Shrinks the size allocated for the children of a class
void class_shrink(struct oo_class *self){
  return;
  self->capacity = self->capacity/3;
  printf("shrinking\n");
  struct oo_class **old = self->children;
	
  if ((self->children = realloc(self->children,sizeof(struct oo_class**)*self->capacity))==NULL){
    printf("FAILED realloc\n");
    self->children = old;
    exit(1);
  }  
}

//[DONE]
//Grows the size allocated for the children of a class
void class_grow(struct oo_class *self){
  //printf("-Growing %s-\n",self->name);
  self->capacity *=2;
  struct oo_class **old = self->children;
	
  if ((self->children = realloc(self->children,sizeof(struct oo_class**)*self->capacity))==NULL){
    printf("FAILED realloc\n");
    self->children = old;
    exit(1);
  }  
}

//[DONE]
struct oo_class* get_class(struct oo_class *parent, const char *name){
  if (strcmp(parent->name,name)==0){
    return parent;
  }
  
  assert(parent->children!=NULL);
  for (size_t i = 0 ; i<parent->size;i++){
    if (class_contains_class(parent->children[i],name)){
      return get_class(parent->children[i],name);
    }
  }
  
  return NULL;
}



//[DONE]
//returns a pointer to the removed class
struct oo_class *hierarchy_remove(struct oo_hierarchy *self,const char *name){
  if (strcmp(name,"Object")==0){ //can't remove Object
    printf("You can't delete Object you idiot, read the rules.\n");
    return NULL;
  }
	
  if (!class_contains_class(self->root,name)){
    printf("The class '%s' isn't available, please try again later.\n",name);
    return NULL; 
  }
	
  struct oo_class *rem =  get_class(self->root,name);
  struct oo_class *rem_parent = rem->parent;
  size_t index;
	
  for (index = 0; index<rem_parent->size;index++){
    if (strcmp(rem_parent->children[index]->name,name)==0){
      break;
    }
  }
	
  for (size_t i = index; i<rem_parent->size-1;i++){
    rem_parent->children[i]=rem_parent->children[i+1];
  }
	
  rem_parent->size--;
  if (rem_parent->size <= rem_parent->capacity/3){
    class_shrink(rem_parent);
  }
	
  return rem;
}



//[DONE]
//set a class as child of another (doesn't 'move' the class only assigns it as child of self)
void class_set_child(struct oo_class *self,struct oo_class *child){
  if (self==NULL || child ==NULL){
    printf("Error null value in set_child\n");
    return;
  }
	
  if (self->size + 1 == self->capacity){
    class_grow(self);
  }
	
  size_t index=0;
  for(index = 0; index<self->size;index++){
    if (strcmp(self->children[index]->name,child->name)>0){
      break;
    }
  }
	
  for (size_t i = self->size+1; i>index;i--){
    self->children[i]=self->children[i-1];
  }

  child->parent = self;
  self->children[index]=child;
  self->size++;
}

//[DONE]
void class_add_child(struct oo_class *self,const char *name){  
  struct oo_class *new =malloc(sizeof(struct oo_class));
  new->parent = self;
  new->size = 0;
  new->capacity = 10;
  
  //Set children class/name
  new->children = calloc(new->capacity,sizeof(struct oo_class**));
  new->name = malloc(strlen(name)+1 * sizeof(char));
  strcpy(new->name,name);
  class_set_child(self,new);
}

//REQUESTED[DONE]
bool hierarchy_add_child(struct oo_hierarchy *self, const char *parent, const char *child) {
  if (!class_contains_class(self->root,parent)){
    return false;
  }
  
  struct oo_class *parent_class = get_class(self->root,parent);
  if(class_contains_class(self->root,child)){
    printf("Error : Hierarchy already contains %s !\n",child);
    return false;
  }
  
  assert(parent_class!=NULL);
  
  class_add_child(parent_class,child);
  if (!class_children_is_sorted(parent_class)){
    //class_children_sort(parent_class);
  }
  assert(class_contains_class(parent_class,child));

  //printf("Added %s to %s \n",child,parent);
  return true;
}



//[DONE]
bool class_has_parent(const struct oo_class *self,const char *name){
  if (strcmp(self->name,name)==0){
    return true;
  }
  
  if (self->parent ==NULL){
    return false;
  }
  
  return class_has_parent(self->parent,name);
}

//REQUESTED [DONE]
bool hierarchy_is_child_of(const struct oo_hierarchy *self, const char *parent, const char *child) {

  if (self->root == NULL){
    return false; //Return false because the root class doesn't exists
  }
  
  if (!class_contains_class(self->root,child)){
    return false; //Return false because the child doesn't exists
  }
  
  struct oo_class *child_class = get_class(self->root,child);
  assert(child_class!=NULL);
  return class_has_parent(child_class,parent);
}



//REQUESTED[DONE]
bool hierarchy_delete(struct oo_hierarchy *self, const char *name) {
  struct oo_class *rem = hierarchy_remove(self,name);
  if (rem!=NULL){
    printf("Deleting %s\n",name);
    free(rem);
    return true;
  }
	
  return false;
}



//[DONE]
void class_sort(struct oo_class *self){
	class_children_sort(self); //Sort the children of the root class
	
	if(class_count_children(self) == 0){return;}
	
	class_children_sort(self);
	for(size_t i = 0; i<self->size; ++i){
		class_sort(self->children[i]); //Sort every class (if it exists) of the hierarchy recursively
	}
}



//REQUESTED[DONE]
bool hierarchy_rename(struct oo_hierarchy *self, const char *old_name, const char *new_name) {
  if(strcmp(old_name,"Object")==0){printf("You can't rename Object. He is the root of all beings.\n");return false;}
	
  if(!class_contains_class(self->root,old_name)){return false;} //Check if the class we want to rename exists
	
	struct oo_class *curr = get_class(self->root,old_name);
  if (strcmp(curr->name,new_name)==0){return true;}
  free(curr->name);
	
  printf("Renaming '%s' to '%s'...\n",old_name,new_name);
  curr->name = calloc(strlen(new_name)+1,sizeof(char));
  strcpy(curr->name,new_name);
	
  if (!class_children_is_sorted(curr->parent)){
    class_sort(curr->parent);
  }
  return true;

}



//REQUESTED[DONE]
bool hierarchy_move_as_child_of(struct oo_hierarchy *self, const char *name, const char *parent) {
  if (!class_contains_class(self->root,name)){
    printf("The class '%s' you are trying to move doesn't exist\n",name);
    return false;
  }
	
  if (!class_contains_class(self->root,parent)){
    printf("The destination class you are trying to move %s to doesn't exist\n",parent);
    return false;
  }

  if (strcmp(name,parent)==0){ //hehe easy
    return true;
  }
	
  struct oo_class *target = get_class(self->root,parent);
  assert(target!=NULL);
  if (strcmp("Object",name)==0){
    printf("You can't move Object ! Read the rules.\n");
  }
	
  struct oo_class *curr = get_class(self->root,name);
  if(class_contains_class(curr,parent)){
    printf("You are trying to move a class inside itself, which doen't make any sense.\nI suggest you take a short break.\n");
    return false;
  }
	
  curr = curr->parent;
  struct oo_class *to_move= NULL;
  //int index;
	
  for(size_t index =0; index< curr->size;index++){
    if (strcmp(curr->children[index]->name,name)==0){
      to_move = curr->children[index]; 
      for (size_t i = index; i<curr->size-1;i++){//shift children because removed one at index
        curr->children[i] = curr->children[i+1];
      }
      break;
    }
  }

  curr->size--;
  if (curr->size <= curr->capacity/3){
    class_shrink(curr);
  }
  
  printf("Moving %s in %s\n",to_move->name,target->name);
  class_set_child(target,to_move);


  return true;

}



//[DONE]
//splits the path into a tab (ex : Object/Folder/File --> {"Object","Folder","File"}  )
//and returns the created path
struct tab *split_path(const char *path){
  if (path == NULL){return NULL;}
  //create res tab
  struct tab *res = calloc(1,sizeof(struct tab));
  //copy path into copy for manipulating
  char *copy = malloc((strlen(path)+1)*sizeof(char));
  strcpy(copy,path);
  printf("copy : %s\n",copy);
	
  int size = 0;
  char **tab=calloc(1,sizeof(char*));
  char *ptr = strtok(copy,"/");
	
  while(ptr != NULL){
    size++;
    tab = realloc(tab,size* sizeof(char*));
    if (tab == NULL){exit(-1);}//FAILED REALLOC
    tab[size-1] = ptr;
    ptr = strtok(NULL,"/");

  }
	
  tab[size] = NULL;
  res->size = size;
  res->args = tab;
  for (size_t i = 0; i < res->size; ++i){
    printf("Res[%zu] = %s\n", i, res->args[i]);
  }
	
  free(copy);
  return res;
}

//[DONE]
//Returns true if the path tab exists in the hierarchy
bool path_exists(struct oo_hierarchy *self, struct tab *path){
  for(size_t i = path->size-1; i > 0; --i){
		if(!hierarchy_has_class(self,path->args[i]) || !hierarchy_is_child_of(self,path->args[i-1],path->args[i])){
			return false;
		}
	}
	return true;
}

//[DONE]
//Compares the path tab with the hierarchy:
//is the path exists it returns true
//if the path can be created without contradictions (i.e no duplicates of classes), it returns true
//else: it returns false
bool path_is_valid(struct oo_hierarchy *self,struct tab *path){
  if(path_exists(self,path)){
		return true;
	}
		 
	bool ans = false;
	for(size_t i = 0; i > path->size-1; --i){
		if(!hierarchy_has_class(self,path->args[i])){
			ans = true;
		}
		if(hierarchy_has_class(self,path->args[i]) && hierarchy_is_child_of(self,path->args[i-1],path->args[i])){
			ans = true;
		}else{
			ans = false;
		}
	}
	
	return ans;
}

//[]
//returns the class from the path tab
//or NULL if path is not valid
struct oo_class* get_class_from_path(struct oo_hierarchy *self,struct tab *path){
	if(!path_is_valid(self,path)){
			return NULL;
	}
	//struct oo_class *curr = get_class(self);
	return NULL;
}



//[DONE]
//returns the maximum depth of the class (i.e how many levels can we go in)
size_t class_depth(struct oo_class *self){
  if (self->size==0){
    return 0;
  }
	
  int deepest = 0;
  int tmp;
  for (size_t i = 0 ; i<self->size; i++){
    tmp = class_depth(self->children[i]);
    if (tmp > deepest){deepest = tmp;}
  }
  return deepest+1;
}



//REQUESTED[DONE]
bool hierarchy_add_path(struct oo_hierarchy *self, const char *path) {
	struct tab *path_tab = split_path(path);
	
	if(path_is_valid(self,path_tab)){
		if(path_exists(self,path_tab)){
			return true;
		}
	}
  return false;
}



//REQUESTED
bool hierarchy_add_path_as_child_of(struct oo_hierarchy *self, const char *path, const char *parent) {
  return false;
}



//REQUESTED[DONE]
char *hierarchy_get_path_to(const struct oo_hierarchy *self, const char *name) {
  //allocate an int tab of size (depth of root) for successive indexes of children so keep all in memory as we go up
  //go up until parent ==NULL (i.e we reached root) from name and count how many times we go up (n)
  //allocate n*20+n chars of memory (+n for the '/' char in-between names)

  if (strcmp(name,"Object")==0){return "Object";}

  if (!class_contains_class(self->root,name)){printf("The requested class '%s' does not exist!",name);return NULL;}
  int total_size=0;
  size_t max_depth = class_depth(self->root);
  int indexes[max_depth];
  int n = 0;

  struct oo_class *curr = get_class(self->root,name);
  struct oo_class *prev = curr;
  assert(curr!=NULL);
  total_size+=strlen(curr->name)+1;
  while(curr->parent!=NULL){
    curr=curr->parent;
    total_size+=strlen(curr->name)+1;

    for (size_t i = 0; i<curr->size;i++){
      if(strcmp(curr->children[i]->name,prev->name)==0){
        indexes[n]=i;
        break;
      }
    }
    n++;
    prev = curr;
  }

  total_size++;
  char *s = malloc(total_size*sizeof(char));
  curr = self->root;
  strcpy(s,curr->name);

  for (int j =n-1 ;  j>=0;j--){
    strcat(s,"/\0");
    strcat(s,curr->children[indexes[j]]->name);
    curr=curr->children[indexes[j]];
  } 
	struct tab *z = split_path(s);
	
  return s;
}



//REQUESTED[DONE]
void hierarchy_sort(struct oo_hierarchy *self) {
	class_sort(self->root);
}



//[DONE]
void path_print(const struct oo_hierarchy *self,struct oo_class *class, FILE *out){
	if(!class){return;}
	char *path = hierarchy_get_path_to(self,class->name);
	fprintf(out,"%s\n",path);
	if(class->size > 0){
		for (size_t i = 0; i < class->size; i++){
    	path_print(self,class->children[i],out);
    	//fprintf(out,"\n");
   	}
	}
}

//REQUESTED[DONE]
void hierarchy_print(const struct oo_hierarchy *self, FILE *out) {
	if (self->root == NULL){return;}
	
	fprintf(out,"\n____HIERARCHY____[%zu]\n",hierarchy_count_classes(self));
 	path_print(self,self->root,out);
	
	fprintf(out,"\n_________________\n\n");
}



//REQUESTED
bool hierarchy_move(struct oo_hierarchy *self, const char *origin, const char *destination) {
  return false;
}
