
typedef int* gtype_t;
typedef struct ga_individual* individual_t;
typedef struct ga_population* ga_population_t;

//===========================

typedef struct city* city_t;
typedef int* city_array_t;

typedef struct vec_cell{
  vec_cell *next;
  int val;
}vec_cell, *vec_cell_t;

typedef struct vec{
  vec_cell_t header;
  int size;
}vec, *vec_t;

struct city{
  int x;
  int y;
}city;

//===========================

struct ga_individual{
  gtype_t gtype;
  city_array_t ptype;
  double fitness;
  individual_t next;
  int rank;
  int parent1;
  int parent2;
  int cross_point;
};

struct ga_population{
  individual_t genes;
  double *pselect;
  int mutate_count;
  double max_fitness;
  double min_fitness;
  double avg_fitness;
  int population_size;
  int code_length;
  int code_max;
};
