#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "header_tsp.h"
#include "param_tsp.h"
using namespace std;

void* my_malloc(int size){
  void* ptr = malloc(size);
  if(ptr == NULL){
    fprintf(stderr,"Faild to malloc\n");
    exit(1);
  }
  return ptr;
}

gtype_t mk_gtype(int code_length){
  gtype_t gtype = (gtype_t)my_malloc(sizeof(int) * code_length);
  return gtype;
}

vec_t mk_vec(){
  vec_t t = (vec_t)malloc(sizeof(struct vec));
  t->size = 0;
  t->header = NULL;
  return t;
}

void add_vec(vec_t v,int x){
  vec_cell_t p,q,new_vec;
  new_vec = (vec_cell_t)malloc(sizeof(vec_cell_t));
  new_vec->next = NULL;
  new_vec->val = x;
  v->size++;
  if(v->header != NULL){
    for(p = v->header;p != NULL;p = p->next)q = p;
    q->next = new_vec;
  }else{
    v->header = new_vec;
  }
}

int vec_get(vec_t v,int x){
  vec_cell_t p = v->header;
  for(int i = 0;i < x;i++)p = p->next;
  return p->val;
}

int vec_size(vec_t v){
  return v->size;
}

int vec_del(vec_t v,int i){
  vec_cell_t p,q,r;
  int x;
  v->size--;
  if(i != 0){
    p = v->header;
    for(int k = 0;k < i;k++){
      q = p;
      p = p->next;
    }
    r = q->next;
    q->next = r->next;
    x = r->val;
    free(r);
    return x;
  }else{
    p = v->header;
    v->header = p->next;
    x = p->val;
    free(p);
    return x;
  }
}

double distance(city_array_t route,int n_city,city_t *cities){
  int x1,y1,x2,y2;
  double dis = 0.0;
  for(int i = 0;i < n_city;i++){
    x1 = cities[route[i]-1]->x;
    y1 = cities[route[i]-1]->y;
    x2 = cities[route[i+1]-1]->x;
    y2 = cities[route[i+1]-1]->y;
    dis += sqrt(pow(x1-x2,2) + pow(y1-y2,2));
  }
  return dis;
}

int get_n_city(const char *filename){
  FILE *fp;
  int i,x,y;
  fp = fopen(filename,"r");
  if(fp == NULL){
    puts("Can't open file");
    exit(0);
  }
  i = 0;
  while(fscanf(fp,"%d,%d",&x,&y) != EOF)i++;
  return i;
}

city_t mk_city(int x,int y){
  city_t ptr = (city_t)malloc(sizeof(city));
  ptr->x = x;
  ptr->y = y;
  return ptr;
}

int input(const char *filename,city_t* cities){
  // cout << "filename:" << filename << endl;
  FILE *fp;
  int x,y;
  int i = 0;
  // int n_city;
  fp = fopen(filename,"r");
  if(fp == NULL){
    puts("Can't open file");
    exit(0);
  }
  while(fscanf(fp,"%d,%d",&x,&y) != EOF){
    cities[i] = mk_city(x,y);
    i++;
  }
  fclose(fp);
  return i;
}

void print_route(city_array_t route,int n_city){
  for(int i = 0;i < n_city+1;i++){
    if(route[i] < 10)printf("%d",route[i]);
    else printf("_%d_",route[i]);
  }
  // puts("");
}

void copy_route(city_array_t new_city,city_array_t old_city,int n_city){
  for(int i = 0;i < n_city+1;i++){
    new_city[i] = old_city[i];
  }
}

void free_gtype(gtype_t gtype){
  free(gtype);
  return;
}

gtype_t mk_random_gtype(int code_length,int code_max){
  gtype_t ptr = mk_gtype(code_length);
  for(int i = 0;i < code_length;i++){
    ptr[i] = rand() % (code_max + 1);
  }
  return ptr;
}

gtype_t mk_random_gtype(int n_city){
  gtype_t ptr = mk_gtype(n_city);
  ptr[0] = 1;
  for(int i = 1;i < n_city;i++)ptr[i] = rand() % (n_city - i) + 1;
  return ptr;
}

void encode_gtype(double value,gtype_t gtype,int code_length,double minimum,double maximum){
  double gap = maximum - minimum;
  double remain_value = value - minimum;
  double value_of_code;
  int position = 1;
  int pre_code = 0;
  int i = 0;
  int tmp;
  while(i < code_length){
    value_of_code = gap / pow(2,position);
    if(remain_value >= value_of_code){
      gtype[i] = 1;
      remain_value -= value_of_code;
    }else{
      gtype[i] = 0;
    }

    if(GRAY == 1){
      tmp = gtype[i];
      gtype[i] = (pre_code) ^ (gtype[i]);
      pre_code = tmp;
    }
    position++;
    i++;
  }
  return;
}

void decode_gtype(city_array_t route,gtype_t gtype,int n_city){
  int tmp;
  vec_t vec = mk_vec();
  for(int i = 0;i < n_city;i++)add_vec(vec,i+1);
  for(int i = 0;i < n_city;i++){
    tmp = vec_del(vec,gtype[i]-1);
    route[i] = tmp;
  }
  route[n_city] = 1;
  free(vec);
}

void copy_gtype(gtype_t new_gtype,gtype_t old_gtype,int length){
  for(int i = 0;i < length;i++){
    new_gtype[i] = old_gtype[i];
  }
  return;
}

int cross_gtype(gtype_t gtype1,gtype_t gtype2,int length){
  int cross_point = rand() % (length - 1);
  int i = cross_point + 1;
  int tmp;
  while(i < length){
    tmp = gtype1[i];
    gtype1[i] = gtype2[i];
    gtype2[i] = tmp;
    i++;
  }
  return cross_point;
}

int mutate_gtype(gtype_t gtype,int length,int code_max,double pm){
  if(pm < 0.0 || 1.0 < pm){
    printf("%f mutation probability must be from 0.0 to 1.0 \n",pm);
    exit(-1);
  }
  int mutate_point = 0;
  double rm;
  for(int i = 0;i < length;i++){
    rm = (double)rand() / RAND_MAX;
    if(rm < pm){
      gtype[i] = rand() % (code_max + 1);
      mutate_point++;
    }
  }
  return mutate_point;
}

int mutate_gtype(gtype_t gtype,int length,double pm){
  if(pm < 0.0 || 1.0 < pm){
    printf("%f mutation probability must be from 0.0 to 1.0 \n",pm);
    exit(-1);
  }
  int mutate_point = 0;
  double rm;
  for(int i = 1;i < length;i++){
    rm = (double)rand() / RAND_MAX;
    if(rm < pm){
      gtype[i] = rand() % (length - i) + 1;
      mutate_point++;
    }
  }
  return mutate_point;
}

void print_gtype(gtype_t gtype,int length){
  printf("[");
  for(int i = 0;i < length;i++){
    if(gtype[i] < 10)printf("%d",gtype[i]);
    else printf("(%d)",gtype[i]);
  }
  printf("]\n");
}

void switch_gene(individual_t *individual){
  individual_t tmp_ptr1 = (*individual) -> next -> next;
  individual_t tmp_ptr2 = (*individual) -> next;
  (*individual) -> next -> next = *individual;
  (*individual) -> next = tmp_ptr1;
  (*individual) = tmp_ptr2;
  return;
}

individual_t mk_gene(int code_length,int code_max){
  individual_t ptr = (individual_t)malloc(sizeof(ga_individual));
  ptr->gtype = mk_random_gtype(code_length,code_max);
  ptr->ptype = 0;
  ptr->fitness = 0;
  ptr->next = NULL;
  ptr->parent1 = 0;
  ptr->parent2 = 0;
  ptr->cross_point = 0;
  return ptr;
}

individual_t mk_gene(int code_length){
  individual_t ptr = (individual_t)malloc(sizeof(ga_individual));
  ptr->gtype = mk_random_gtype(code_length);
  ptr->ptype = (city_array_t)malloc(sizeof(int) * (code_length + 1)) ;
  ptr->fitness = 0;
  ptr->next = NULL;
  ptr->parent1 = 0;
  ptr->parent2 = 0;
  ptr->cross_point = 0;
  return ptr;
}

void copy_gene(individual_t new_gene,individual_t old_gene,int code_length){
  copy_gtype(new_gene->gtype,old_gene->gtype,code_length);
  copy_route(new_gene->ptype,old_gene->ptype,code_length);
  new_gene->fitness = old_gene->fitness;
  new_gene->parent1 = old_gene->rank;
  new_gene->parent2 = old_gene->rank;
  new_gene->cross_point = code_length - 1;
  return;
}

int mk_children_genes(individual_t child1,individual_t child2,individual_t parent1,individual_t parent2,int code_length,double pm){
  int cross_point,mutateCount;
  copy_gene(child1,parent1,code_length);
  copy_gene(child2,parent2,code_length);
  cross_point = cross_gtype(child1->gtype,child2->gtype,code_length);
  child1->parent1 = parent1->rank;
  child1->parent2 = parent2->rank;
  child1->cross_point = cross_point;

  child2->parent1 = parent2->rank;
  child2->parent2 = parent1->rank;
  child2->cross_point = cross_point;
  mutateCount = mutate_gtype(child1->gtype,code_length,pm);
  mutateCount += mutate_gtype(child2->gtype,code_length,pm);
  return mutateCount;
}

ga_population_t mk_init_ga_population(int population_size,int code_length){
  ga_population_t population = (ga_population_t)malloc(sizeof(ga_population));
  population->pselect = (double*)my_malloc(sizeof(double) * population_size);
  population->mutate_count = 0;
  population->population_size = population_size;
  population->code_length = code_length;
  individual_t list_tale;
  population->genes = mk_gene(code_length);
  list_tale = population->genes;
  for(int i = 1;i < population_size;i++){
    list_tale->next = mk_gene(code_length);
    list_tale = list_tale->next;
  }
  return population;
}

void print_sequence(char ch,int length){
  for(int i = 0;i < length;i++)printf("%c",ch);
}

void print_population(ga_population_t population){
  individual_t member = population->genes;
  int i = 0;
  printf("---------");
  print_sequence('-',population->code_length + 2);
  printf("-------\n");
  printf("#    parents xsite  gtype");
  print_sequence('-',population->code_length - 3);
  printf("fitness\n");

  while(member != NULL){
    printf("%-3d (%3d,%3d) %3d ",i++,member->parent1,member->parent2,member->cross_point);
    print_gtype(member->gtype,population->code_length);
    printf(" %+3.3d %+3.3f\n",*member->ptype,member->fitness);
    print_route(member->ptype,population->code_length);
    puts("");
    member = member->next;
  }
  printf("total mutate %d\n",population->mutate_count);
  return;
}

void print_fitness(ga_population_t population){
  print_route(population->genes->ptype,population->code_length);
  // printf("    ");
  puts("");
  print_gtype(population->genes->gtype,population->code_length);
  printf("%f\n",1 / population->genes->fitness);
  return;
}

int less_than(individual_t individualA,individual_t individualB){
  return (individualA->fitness < individualB->fitness);
}

void calc_pselect(ga_population_t population){
  population->pselect[0] = population->genes->fitness;
  individual_t genes_ptr = population->genes->next;
  for(int i = 1;i < population->population_size;i++){
    population->pselect[i] = population->pselect[i-1] + genes_ptr->fitness;
    genes_ptr = genes_ptr->next;
  }
  for(int i = 0;i < population->population_size;i++){
    population->pselect[i] /= population->pselect[population->population_size-1];
  }
}

individual_t select_parent_roulette(ga_population_t population){
  int i = 0;
  double r;
  individual_t parent;
  r = (double)rand() / RAND_MAX;
  parent = population->genes;
  while(r > population->pselect[i++])parent = parent->next;
  return parent;
}

individual_t select_parent_tournament(ga_population_t population,int tournament_size){
  int pop_t = population->population_size;
  int r,min_t = pop_t;
  individual_t min_selected = NULL;
  individual_t ptr;
  for(int i = 0;i < tournament_size;i++){
    r = rand() % pop_t;
    if(min_t > r)min_t = r;
  }
  ptr = population->genes;
  for(int i = 0;i < min_t;i++)ptr = ptr->next;
  min_selected = ptr;
  return min_selected;
}

individual_t select_parent(ga_population_t population){
  individual_t parent;
  switch(SELECTION_METHOD){
  case 1:
    parent = select_parent_roulette(population);
    break;
  case 2:
    parent = select_parent_tournament(population,TOURNAMENT_SIZE);
    break;
  default:
    fprintf(stderr,"invalid member on SELECTION_METHOD\n");
    exit(1);
  }
  return parent;
}

void normalize_population(ga_population_t population){
  individual_t tmp = population->genes;
  population->max_fitness = population->genes->fitness;
  double avg = 0.0;
  for(int i = 0;i < population->population_size;i++){
    avg += tmp->fitness;
    tmp->rank = i;
    if(tmp->next == NULL){
      population->min_fitness = tmp->fitness;
    }
    tmp = tmp->next;  
  }
  avg = avg / population->population_size;
  population->avg_fitness = avg;
  return;
}

void calc_fitness(ga_population_t population,city_t *cities){
  individual_t ptr = population->genes;
  individual_t next;
  individual_t individual_ptr = NULL;
  individual_t search_ptr = ptr;
  while(ptr != NULL){
    decode_gtype(ptr->ptype,ptr->gtype,population->code_length);
    ptr->fitness = 1 / distance(ptr->ptype,population->code_length,cities);
    next = ptr->next;
    ptr->next = NULL;
    search_ptr = individual_ptr;
    if(search_ptr == NULL || less_than(individual_ptr,ptr)){
      ptr->next = individual_ptr;
      individual_ptr = ptr;
    }else{
      while(search_ptr->next != NULL){
        if(less_than(search_ptr->next,ptr))break;
        search_ptr = search_ptr->next;
      }
      ptr->next = search_ptr->next;
      search_ptr->next = ptr;
    }
    ptr = next;
  }
  population->genes = individual_ptr;
}

void generate_population(ga_population_t new_population,ga_population_t old_population,double gap,double elite_rate,double mutate_prob,double crossover_prb){
  int num_of_remain = (int)(old_population->population_size * (1-gap));
  int num_of_elite = (int)(num_of_remain * elite_rate);
  int generated;
  double rand_double;
  individual_t old_gene = old_population->genes;
  individual_t new_gene = new_population->genes;
  calc_pselect(old_population);
  // chose elite
  for(generated = 0;generated < num_of_elite;generated++){
    copy_gene(new_gene,old_gene,old_population->code_length);
    old_gene = old_gene->next;
    new_gene = new_gene->next;
  }
  // others
  for(;generated < num_of_remain;generated++){
    copy_gene(new_gene,select_parent(old_population),old_population->code_length);
    new_gene = new_gene->next;
  }
  new_population->mutate_count = 0;
  if((old_population->population_size - generated) % 2){
    copy_gene(new_gene,select_parent(old_population),old_population->code_length);
    new_population->mutate_count += mutate_gtype(new_gene->gtype,old_population->code_length,mutate_prob);
    new_gene = new_gene->next;
    generated++;
  }
  for(;generated < old_population->population_size;generated += 2){
    rand_double = (double)rand() / RAND_MAX;
    if(rand_double < crossover_prb){
      new_population->mutate_count += mk_children_genes(new_gene,new_gene->next,select_parent(old_population),select_parent(old_population),old_population->code_length,mutate_prob);
      new_gene = new_gene->next;
    }else{
      copy_gene(new_gene,select_parent(old_population),old_population->code_length);
      new_population->mutate_count += mutate_gtype(new_gene->gtype,old_population->code_length,mutate_prob);
      new_gene = new_gene->next;
      copy_gene(new_gene,select_parent(old_population),old_population->code_length);
      new_gene = new_gene->next;
    }
  }
}

void ga_main(const char *argv){
  srand(time(NULL));
  int n_city = get_n_city(argv);
  city_t* cities = (city_t*)malloc(sizeof(city_t) * n_city);
  cout << "city : " << input(argv,cities) << endl;
  ga_population_t parent_group = mk_init_ga_population(POP,n_city);
  ga_population_t child_group = mk_init_ga_population(POP,n_city);
  ga_population_t tmp;
  // printf("#        Route,            Gtype,     Distance\n");
  for(int i = 0;i < GENERATION;i++){
    calc_fitness(parent_group,cities);
    normalize_population(parent_group);
    if(PRINT_GROUP == 1)print_population(parent_group);
    if(PRINT_FITNESS == 1){
      printf("\n第%3d世代\n",i + 1);
      print_fitness(parent_group);
    }
    generate_population(child_group,parent_group,GAP,ELITE_RATE,P_MUTATE,P_CROSS);
    parent_group = child_group;
  }
}

int main(int argc,char *argv[]){
  ga_main(argv[1]);
  return 0;
}
