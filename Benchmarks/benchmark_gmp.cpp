 #include<benchmark/benchmark.h>
 #include<bits/stdc++.h>
 #include "include/helper.hpp"
 #include<gmp.h>
 #include<mpfr.h>
 
 #define M 100000
 #define precision 40

 
 static void BM_add_int(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+1];
     char* ar_b=new char[n+1];
	 mpz_t a,b,r;
	 mpz_init(a);
	 mpz_init(b);
	 mpz_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
         std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
		 mpz_set_str(a,ar_a,10);
		 mpz_set_str(b,ar_b,10);

		 state.ResumeTiming();

		 mpz_add(r,a,b);

	 }
	 
	 mpz_clear(a);
	 mpz_clear(b);
	 mpz_clear(r);
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_add_real(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpf_t a,b,r;
	 mpf_init(a);
	 mpf_init(b);
	 mpf_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpf_set_str(a,ar_a,10);
		 mpf_set_str(b,ar_b,10);
		 
		 
		 state.ResumeTiming();

		 mpf_add(r,a,b);
	 }
	 
	 mpf_clear(a);
	 mpf_clear(b);
	 mpf_clear(r);
	 delete(ar_a);
	 delete(ar_b);
	 
}
static void BM_multiply_int(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+1];
     char* ar_b=new char[n+1];
	 mpz_t a,b,r;
	 mpz_init(a);
	 mpz_init(b);
	 mpz_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
         std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
		 mpz_set_str(a,ar_a,10);
		 mpz_set_str(b,ar_b,10);

		 state.ResumeTiming();

		 mpz_mul(r,a,b);

	 }
	 
	 mpz_clear(a);
	 mpz_clear(b);
	 mpz_clear(r);
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_multiply_real(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpf_t a,b,r;
	 mpf_init(a);
	 mpf_init(b);
	 mpf_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpf_set_str(a,ar_a,10);
		 mpf_set_str(b,ar_b,10);
		 
		 
		 state.ResumeTiming();

		 mpf_mul(r,a,b);
	 }
	 
	 mpf_clear(a);
	 mpf_clear(b);
	 mpf_clear(r);
	 delete(ar_a);
	 delete(ar_b);
	 
}
static void BM_divide_int(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+1];
     char* ar_b=new char[n+1];
	 mpz_t a,b,r;
	 mpz_init(a);
	 mpz_init(b);
	 mpz_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
         std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
		 mpz_set_str(a,ar_a,10);
		 mpz_set_str(b,ar_b,10);

		 state.ResumeTiming();

		 mpz_fdiv_q(r,a,b);

	 }
	 
	 mpz_clear(a);
	 mpz_clear(b);
	 mpz_clear(r);
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_divide_real(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpf_t a,b,r;
	 mpf_init(a);
	 mpf_init(b);
	 mpf_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpf_set_str(a,ar_a,10);
		 mpf_set_str(b,ar_b,10);
		 
		 
		 state.ResumeTiming();

		 mpf_div(r,a,b);
	 }
	 
	 mpf_clear(a);
	 mpf_clear(b);
	 mpf_clear(r);
	 delete(ar_a);
	 delete(ar_b);
	 
}

static void BM_sqrt(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpf_t a,r;
	 mpf_init(a);
	 mpf_init(r);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpf_set_str(a,ar_a,10);
		 
		 state.ResumeTiming();

		 mpf_sqrt(r,a);
	 }
	 
	 mpf_clear(a);
	 mpf_clear(r);
	 delete(ar_a);
	 
}

 BENCHMARK(BM_add_int)->RangeMultiplier(10)->Range(10,M);
 BENCHMARK(BM_add_real)->RangeMultiplier(10)->Range(10,M);
 
 BENCHMARK(BM_multiply_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_multiply_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_divide_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_divide_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_sqrt)->RangeMultiplier(8)->Range(8,1024);
 
 BENCHMARK_MAIN();
