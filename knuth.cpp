#include <stdio.h>
#include <omp.h>

#include <stdlib.h>
#include <stdint.h>

#ifdef GMMP

#include <gmp.h>

#else

#define LEN 6
#define BASE 1000000000

typedef  uint32_t mpz_t[LEN];

void mpz_init(mpz_t a) {
 for(int i=0;i<LEN;i++)
  a[i]=0;
}

void mpz_set(mpz_t a, mpz_t b) {
 for(int i=0;i<LEN;i++)
  a[i]=b[i];
}

void mpz_add(mpz_t a, mpz_t b, mpz_t c) {
 uint32_t carry=0;
 for(int i=0;i<LEN;i++) {
  a[i]=b[i]+c[i]+carry;
  if(a[i]>=BASE){ carry=1; a[i]-=BASE;} else carry=0;
 }
}

void mpz_set_si(mpz_t a, int si) {
 a[0]=si;
 for(int i=1;i<LEN;i++)
 	a[i]=0;
}

void mpz_get_str(char* str, int base, mpz_t a) {
 for(int i=LEN-1;i>=0;i--)
 	printf("%09d ",a[i]);
 *str=' ';
 *(str+1)='\0';
}

#endif


// Knuth algorithm

void calc_n_paths_by_min_set(int a_dash, mpz_t** current_state,
			   mpz_t** n_paths_by_a_dash_and_min_set,
			   int n, int n_sets) {
  mpz_t n_paths;
  mpz_init(n_paths);
  for (int a = 0; a < n+1; a++) {
    for (int set_index = 0; set_index < n_sets; set_index++ ) {
      mpz_set(n_paths, current_state[a][set_index]);
      int minimal_set = set_index;
      int a_mask=0, t;
      if(a<a_dash-1){
        a_mask = ((~((1<<a)-1)) & ((1<<a_dash)-1));
        t = (set_index & a_mask)<< 1;
        minimal_set |= t & a_mask;
      } else if (a>a_dash+1) {
        a_mask = (((1<<a)-1) & (~((1<<a_dash)-1)));
        t = (set_index & a_mask)>> 1;
        minimal_set |= t & a_mask;
      }

      mpz_add(n_paths_by_a_dash_and_min_set[a_dash][minimal_set],
	      n_paths_by_a_dash_and_min_set[a_dash][minimal_set],
	      n_paths);
    }
  }
}

void sums(mpz_t *a, int n, mpz_t *b) {
  if (n <= 0) {
    mpz_set(*b,*a);
    return;
  }
  int m = 1 << (n - 1);
  sums(a, n - 1, b);
  sums(a + m, n - 1, b + m);
  for (int i = 0; i < m; i++) {
      mpz_add(b[m + i], b[m+i], b[i]);
  }
}

void assign_n_paths_in_current_state(int a_dash, mpz_t** current_state,
				     mpz_t** n_paths_by_a_dash_and_min_set,
				     int n_sets, int n) {
  sums(n_paths_by_a_dash_and_min_set[a_dash], n, current_state[a_dash]);
}


void n_kings_placements(int n, mpz_t n_placements) {
#ifdef _OPENMP
  omp_set_dynamic(0);
  omp_set_num_threads(n+1);
#endif
  int n_sets = 1 << n;
  printf("n = %d, n_sets = %d\n", n, n_sets);
  mpz_t** n_paths_by_a_dash_and_min_set = new mpz_t*[n + 1];
  for (int i = 0; i < n + 1; i++ ) {
    n_paths_by_a_dash_and_min_set[i] = new mpz_t[n_sets];
    for (int j = 0; j < n_sets; j++ ) {
      mpz_init(n_paths_by_a_dash_and_min_set[i][j]);
    }
  }

  mpz_t** current_state = new mpz_t*[n + 1];
  for (int i = 0; i < n + 1; i++ ) {
    current_state[i] = new mpz_t[n_sets];
    for (int j = 0; j < n_sets; j++ ) {
     /* Every state of first line has 1 path */
        mpz_init(current_state[i][j]);
	mpz_set_si(current_state[i][j], 1);
    }
  }  

  mpz_t n_paths;
  mpz_init(n_paths);
  
  for (int n_iter = 1; n_iter < n; n_iter++ ) {
    printf("starting iter %d, left %d\n", n_iter, n - n_iter - 1);
    fflush(stdout);
    for (int i = 0; i < n + 1; i++ ) {
      for (int j = 0; j < n_sets; j++ ) {
	mpz_set_si(n_paths_by_a_dash_and_min_set[i][j], 0);
      }
    }


    int a_dash;
#pragma omp parallel shared(n_paths_by_a_dash_and_min_set) private(a_dash)
    {
#pragma omp for
      for (a_dash = 0; a_dash < n + 1; a_dash++) {
	calc_n_paths_by_min_set(a_dash, current_state, n_paths_by_a_dash_and_min_set,
				n, n_sets);
      }
    }

    for (int i = 0; i < n + 1; i++ ) {
      for (int j = 0; j < n_sets; j++ ) {
	mpz_set_si(current_state[i][j], 0);
      }
    }
#pragma omp parallel shared(current_state) private(a_dash)
    {
#pragma omp for
      for (a_dash = 0; a_dash < n + 1; a_dash++ ) {
	assign_n_paths_in_current_state(a_dash, current_state,
					n_paths_by_a_dash_and_min_set,
					n_sets, n);
      }
    }

  }
  mpz_set_si(n_paths, 0);
  for (int a = 0; a < n + 1; a++) {
      for (int set_index = 0; set_index < n_sets; set_index++ ) {
	mpz_add(n_paths, n_paths, current_state[a][set_index]);
      }
  }
  mpz_set(n_placements, n_paths);
}


int main(int argc, char* argv[]) {
  if (argc == 2) {
    int n = atoi(argv[1]);
    char str[1000];
    mpz_t n_placements;
    mpz_init(n_placements);
    n_kings_placements(n, n_placements);
    mpz_get_str(str, 10, n_placements);
    printf("%s\n", str);
  } else {
    printf("Usage: knuth N");
  }
}
