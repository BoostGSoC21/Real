 #include<benchmark/benchmark.h>
 #include<mpdecimal.h>
 #include "include/helper.hpp"
 #define M 100000
 #define precision 40

mpd_t *a,*b;
mpd_t *result;
mpd_context_t ctx;
		 
char *rstring;
char status_str[MPD_MAX_FLAG_STRING];		 

static void BM_add_int(benchmark::State& state){		 	 

     mpd_init(&ctx, precision);
     int n=state.range(0);
     char* ar_a=new char[n+1];
     char* ar_b=new char[n+1];
     
	 for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

	     

		 state.ResumeTiming();

		 mpd_add(result, a, b, &ctx);
		 
		 
		 //Stoping timer to freeup memory 
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
	 
}

static void BM_add_real(benchmark:: State& state){

     mpd_init(&ctx, precision);
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	for(auto _:state){
		
		//Pausing timer for variable declaration and constructors.		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

		 state.ResumeTiming();

		 mpd_add(result, a, b, &ctx);
		 
		 //Pausing timer to free up memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_multiply_int(benchmark:: State& state){

    int n=state.range(0);
	char* ar_a=new char[n+1];
	char* ar_b=new char[n+1];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

		 state.ResumeTiming();
 
		 mpd_mul(result, a, b, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_multiply_real(benchmark:: State& state){

    int n=state.range(0);
	char* ar_a=new char[n+2];
	char* ar_b=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

		 state.ResumeTiming();
 
		 mpd_mul(result, a, b, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);

		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_divide_int(benchmark:: State& state){

    int n=state.range(0);
	char* ar_a=new char[n+1];
	char* ar_b=new char[n+1];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomIntegerOfLen(state.range(0));
		 std::string str_b=randomIntegerOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

	     

		 state.ResumeTiming();

		 mpd_div(result, a, b, &ctx);
		 
		 
		 //Stoping timer to freeup memory 
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_divide_real(benchmark:: State& state){

    int n=state.range(0);
	char* ar_a=new char[n+2];
	char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

		 state.ResumeTiming();
 
		 mpd_div(result, a, b, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);

		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_power(benchmark::State& state){

    int n=state.range(0);
	char* ar_a=new char[n+2];
	char* ar_b=new char[n+2];
	 
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     b = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     mpd_set_string(b, ar_b, &ctx);

		 state.ResumeTiming();
 
		 mpd_pow(result, a, b, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(b);
		 mpd_del(result);

		 state.ResumeTiming();
	 }
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_sqrt(benchmark::State& state){

    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);

	     mpd_set_string(a, ar_a, &ctx);

		 state.ResumeTiming();
 
		 mpd_sqrt(result, a, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
}

static void BM_exp(benchmark::State& state){

    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     
		 state.ResumeTiming();
 
 
		 mpd_exp(result, a, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
}

static void BM_log(benchmark::State& state){

    int n=state.range(0);
	char* ar_a=new char[n+2];
	
	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     mpd_set_string(a, ar_a, &ctx);
	     
		 state.ResumeTiming();
 
 
		 mpd_ln(result, a, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 
		 mpd_del(a);
		 mpd_del(result);
		 
		 state.ResumeTiming();
	 }
	 delete(ar_a);
}

/*static void BM_sin(benchmark::State& state){

	for(auto _:state){
		 
		 //Pausing timer for variable declaration and constructors.
		 state.PauseTiming();
         
		 char* str_a=toCharArray(randomRealOfLen(state.range(0)));
		 
	     ctx.traps = 0;

	     result = mpd_new(&ctx);
	     a = mpd_new(&ctx);
	     mpd_set_string(a, str_a, &ctx);
	     
		 state.ResumeTiming();
 
 
		 mpd_exp(result, a, &ctx);
		 
		 //Pausing timer to freeup memory
		 state.PauseTiming();
		 mpd_del(a);
		 mpd_del(result);
		 delete(str_a);
		 state.ResumeTiming();
	 }
}

static void BM_cos(benchmark::State& state){
	for(auto _:state){
		 
		 state.PauseTiming();

		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 int n=str_a.size();
		 
		 char ar_a[n+1];
		 for(int i=0;i<n;++i)
		    ar_a[i]=str_a[i];
		    
		 Real a(ar_a);

		 state.ResumeTiming();

		 Real res=cos(a);
	 }
}

static void BM_tan(benchmark::State& state){
	for(auto _:state){
		 
		 		 state.PauseTiming();

		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 int n=str_a.size();
		 
		 char ar_a[n+1];
		 for(int i=0;i<n;++i)
		    ar_a[i]=str_a[i];
		    
		 Real a(ar_a);

		 state.ResumeTiming();

         Real res=tan(a);
	 }
}
*/

 BENCHMARK(BM_add_int)->RangeMultiplier(10)->Range(10,M);
 BENCHMARK(BM_add_real)->RangeMultiplier(10)->Range(10,M);
 
 BENCHMARK(BM_multiply_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_multiply_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_divide_int)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_divide_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_power)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_sqrt)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_exp)->RangeMultiplier(8)->Range(8,1024);
 
/* BENCHMARK(BM_sin)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_cos)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_tan)->RangeMultiplier(8)->Range(8,1024);
*/ 
 BENCHMARK_MAIN();
