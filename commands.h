/**************************************************************
***************************************************************
*** 
***   commands.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This file sets up the command arrays.
***
***************************************************************
**************************************************************/

enum {type_double, type_int, type_short_int, type_string};

double arg_double[32];
int arg_int[32];
short int arg_short_int[32];
char arg_string[32][32];

// clusters
void cmd_clusters();
// data
void cmd_abs();
void cmd_add();
void cmd_avg();
void cmd_bins();
void cmd_breg();
void cmd_cmplat();
void cmd_cnt();
void cmd_conste();
void cmd_dd();
void cmd_dindex();
void cmd_disp();
void cmd_div();
void cmd_divarray();
void cmd_dt();
void cmd_filerng();
void cmd_filespins();
void cmd_flt();
void cmd_index();
void cmd_initstat();
void cmd_log10();
void cmd_minmax();
void cmd_mul();
void cmd_prob();
void cmd_rdm();
void cmd_receive();
void cmd_sd();
void cmd_send();
void cmd_stdev();
void cmd_subarray();
void cmd_surv();
void cmd_szd();
void cmd_tag();
void cmd_tcollect();
void cmd_text();
// file
void cmd_load();
void cmd_save();
// list
void cmd_dl();
void cmd_sl();
// loop
void cmd_lp();
void cmd_lpseed();
void cmd_lpseed64();
void cmd_randinit();
void cmd_sizelin();
void cmd_tsc();
void cmd_tsc64();
// plot
void cmd_active();
void cmd_animate();
void cmd_animatert();
void cmd_bregplot();
void cmd_bregplotm();
void cmd_clspins();
void cmd_spins();
void cmd_xy();
// run
void cmd_run();
// save
void cmd_adddata();
void cmd_brdim();
void cmd_chksameg();
void cmd_constean();
void cmd_constebr();
void cmd_fa();
void cmd_fbr();
void cmd_fcl();
void cmd_fe();
void cmd_fm();
void cmd_fs();
void cmd_ft();
void cmd_numbr();
void cmd_perc();
void cmd_plotfbr();
void cmd_pseudobl();
void cmd_svlat();
void cmd_svseed();
void cmd_svseed64();
// set
void cmd_af();
void cmd_altbias();
void cmd_bc();
void cmd_bias();
void cmd_blkinit();
void cmd_block();
void cmd_ev();
void cmd_ldinit();
void cmd_ldlat();
void cmd_randlat();
void cmd_seed();
void cmd_seed64();
void cmd_settsc();
void cmd_size();
void cmd_sp();
void cmd_time();
// step
void cmd_avgtdrop();
void cmd_chk();
void cmd_del();
void cmd_edrop();
void cmd_flips();
void cmd_force();
void cmd_forceend();
void cmd_forceenda();
void cmd_getcutoff();
void cmd_intfcom();
void cmd_latrng();
void cmd_longdrop();
void cmd_magstop();
void cmd_stop();
void cmd_stoplong();
void cmd_svact();
void cmd_svconste();
void cmd_sven();
void cmd_svmag();
void cmd_svtm();
void cmd_svtmdrop();
void cmd_tdropvst();

int main();

char *cmd_name[]=
{
	/* clusters */
	"clusters",
	/* data */
	"abs",
	"add",
	"avg",
	"bins",
	"breg",
	"cmplat",
	"cnt",
	"conste",
	"dd",
	"dindex",
	"disp",
	"div",
	"divarray",
	"dt",
	"filerng",
	"filespins",
	"flt",
	"index",
	"initstat",
	"log10",
	"minmax",
	"mul",
	"prob",
	"rdm",
	"receive",
	"sd",
	"send",
	"stdev",
	"subarray",
	"surv",
	"szd",
	"tag",
	"tcollect",
	"text",
	/* file */
	"load",
	"save",
	/* list */
	"dl",
	"sl",
	/* loop */
	"lp",
	"lpseed",
	"lpseed64",
	"randinit",
	"sizelin",
	"tsc",
	"tsc64",
	/* plot */
	"active",
	"animate",
	"animatert",
	"bregplot",
	"bregplotm",
	"clspins",
	"spins",
	"xy",
	/* run */
	"run",
	/* save */
	"adddata",
	"brdim",
	"chksameg",
	"constean",
	"constebr",
	"fa",
	"fbr",
	"fcl",
	"fe",
	"fm",
	"fs",
	"ft",
	"numbr",
	"perc",
	"plotfbr",
	"pseudobl",
	"svlat",
	"svseed",
	"svseed64",
	/* set */
	"af",
	"altbias",
	"bc",
	"bias",
	"blkinit",
	"block",
	"ev",
	"ldinit",
	"ldlat",
	"randlat",
	"seed",
	"seed64",
	"settsc",
	"size",
	"sp",
	"time",
	/* step */
	"avgtdrop",
	"chk",
	"del",
	"edrop",
	"flips",
	"force",
	"forceend",
	"forceenda",
	"getcutoff",
	"intfcom",
	"latrng",
	"longdrop",
	"magstop",
	"stop",
	"stoplong",
	"svact",
	"svconste",
	"sven",
	"svmag",
	"svtm",
	"svtmdrop",
	"tdropvst",
	/* exit / quit */
	"exit",
	"quit"
};	
void (*functions[])()=
{
	/* clusters */
	cmd_clusters,
	/* data */
	cmd_abs,
	cmd_add,
	cmd_avg,
	cmd_bins,
	cmd_breg,
	cmd_cmplat,
	cmd_cnt,
	cmd_conste,
	cmd_dd,
	cmd_dindex,
	cmd_disp,
	cmd_div,
	cmd_divarray,
	cmd_dt,
	cmd_filerng,
	cmd_filespins,
	cmd_flt,
	cmd_index,
	cmd_initstat,
	cmd_log10,
	cmd_minmax,
	cmd_mul,
	cmd_prob,
	cmd_rdm,
	cmd_receive,
	cmd_sd,
	cmd_send,
	cmd_stdev,
	cmd_subarray,
	cmd_surv,
	cmd_szd,
	cmd_tag,
	cmd_tcollect,
	cmd_text,
	/* file */
	cmd_load,
	cmd_save,
	/* list */
	cmd_dl,
	cmd_sl,
	/* loop */
	cmd_lp,
	cmd_lpseed,
	cmd_lpseed64,
	cmd_randinit,
	cmd_sizelin,
	cmd_tsc,
	cmd_tsc64,
	/* plot */
	cmd_active,
	cmd_animate,
	cmd_animatert,
	cmd_bregplot,
	cmd_bregplotm,
	cmd_clspins,
	cmd_spins,
	cmd_xy,
	/* run */
	cmd_run,
	/* save */
	cmd_adddata,
	cmd_brdim,
	cmd_chksameg,
	cmd_constean,
	cmd_constebr,
	cmd_fa,
	cmd_fbr,
	cmd_fcl,
	cmd_fe,
	cmd_fm,
	cmd_fs,
	cmd_ft,
	cmd_numbr,
	cmd_perc,
	cmd_plotfbr,
	cmd_pseudobl,
	cmd_svlat,
	cmd_svseed,
	cmd_svseed64,
	/* set */
	cmd_af,
	cmd_altbias,
	cmd_bc,
	cmd_bias,
	cmd_blkinit,
	cmd_block,
	cmd_ev,
	cmd_ldinit,
	cmd_ldlat,
	cmd_randlat,
	cmd_seed,
	cmd_seed64,
	cmd_settsc,
	cmd_size,
	cmd_sp,
	cmd_time,
	/* step */
	cmd_avgtdrop,
	cmd_chk,
	cmd_del,
	cmd_edrop,
	cmd_flips,
	cmd_force,
	cmd_forceend,
	cmd_forceenda,
	cmd_getcutoff,
	cmd_intfcom,
	cmd_latrng,
	cmd_longdrop,
	cmd_magstop,
	cmd_stop,
	cmd_stoplong,
	cmd_svact,
	cmd_svconste,
	cmd_sven,
	cmd_svmag,
	cmd_svtm,
	cmd_svtmdrop,
	cmd_tdropvst,
	/* exit / quit */
	0,
	0
};
int num_args[]=
{
	/* clusters */
		/* clusters */	0,
	/* data */
		/* abs */	2,
		/* add */	3,
		/* avg */	1,
		/* bins */	2,
		/* breg */	3,
		/* cmplat */	6,
		/* cnt */	3,
		/* conste */	3,
		/* dd */	1,
		/* dindex */	0,
		/* disp */	3,
		/* div */	3,
		/* divarray */	3,
		/* dt */	0,
		/* filerng */	1,
		/* filespins */	1,
		/* flt */	3,
		/* index */	2,
		/* initstat */	0,
		/* log10 */	2,
		/* minmax */	1,
		/* mul */	3,
		/* prob */	7,
		/* rdm */	2,
		/* receive */	0,
		/* sd */	0,
		/* send */	0,
		/* stdev */	1,
		/* subarray */	3,
		/* surv */	2,
		/* szd */	1,
		/* tag */	2,
		/* tcollect */	6,
		/* text */	2,
	/* file */
		/* load */	3,
		/* save */	2,
	/* list */
		/* dl */	2,
		/* sl */	1,
	/* loop */
		/* lp */	2,
		/* lpseed */	3,
		/* lpseed64 */	4,
		/* randinit */	2,
		/* sizelin */	4,
		/* tsc */	1,
		/* tsc64 */	1,
	/* plot */
		/* active */	5,
		/* animate */	10,
		/* animatert */	13,
		/* bregplot */	3,
		/* bregplotm */	3,
		/* clspins */	3,
		/* spins */	6,
		/* xy */	4,
	/* run */
		/* run */	0,
	/* save */
		/* adddata */	4,
		/* brdim */	5,
		/* chksameg */	3,
		/* constean */	9,
		/* constebr */	9,
		/* fa */	3,
		/* fbr */	3,
		/* fcl */	4,
		/* fe */	3,
		/* fm */	3,
		/* fs */	3,
		/* ft */	3,
		/* numbr */	3,
		/* perc */	6,
		/* plotfbr */	2,
		/* pseudobl */	7,
		/* svlat */	3,
		/* svseed */	3,
		/* svseed64 */	4,
	/* set */
		/* af */	1,
		/* altbias */	1,
		/* bc */	3,
		/* bias */	1,
		/* blkinit */	1,
		/* block */	7,
		/* ev */	9,
		/* ldinit */	0,
		/* ldlat */	0,
		/* randlat */	1,
		/* seed */	1,
		/* seed64 */	2,
		/* settsc */	0,
		/* size */	3,
		/* sp */        0,
		/* time */	1,
	/* step */
		/* avgtdrop */	4,
		/* chk */	1,
		/* del */	2,
		/* edrop */	3,
		/* flips */	3,
		/* force */	2,
		/* forceend */	5,
		/* forceenda */	5,
		/* getcutoff */	5,
		/* intfcom */	5,
		/* latrng */	2,
		/* longdrop */	4,
		/* magstop */	2,
		/* stop */	2,
		/* stoplong */	4,
		/* svact */	3,
		/* svconste */	5,
		/* sven */	3,
		/* svmag */	3,
		/* svtm */	3,
		/* svtmdrop */	3,
		/* tdropvst */	4,
	/* exit / quit */
		/* exit */	0,
		/* quit */	0
};
int arg_types[][32]=
{
	/* clusters */
		/* clusters */	{0},
	/* data */
		/* abs */	{type_short_int, type_short_int},
		/* add */	{type_short_int, type_short_int, type_double},
		/* avg */	{type_short_int},
		/* bins */	{type_short_int, type_int},
		/* breg */	{type_short_int, type_short_int, type_int},
		/* cmplat */	{type_short_int, type_short_int, type_int, type_int, type_int, type_int},
		/* cnt */	{type_short_int, type_double, type_double},
		/* conste */	{type_short_int, type_short_int, type_short_int},
		/* dd */	{type_short_int},
		/* dindex */	{0},
		/* disp */	{type_short_int, type_int, type_int},
		/* div */	{type_short_int, type_short_int, type_double},
		/* divarray */	{type_short_int, type_short_int, type_short_int},
		/* dt */	{0},
		/* filerng */	{type_string},
		/* filespins */	{type_string},
		/* flt */	{type_short_int, type_double, type_double},
		/* index */	{type_short_int, type_int},
		/* initstat */	{0},
		/* log10 */	{type_short_int, type_short_int},
		/* minmax */	{type_short_int},
		/* mul */	{type_short_int, type_short_int, type_double},
		/* prob */	{type_short_int, type_short_int, type_short_int, type_double, type_double, type_int, type_int},
		/* rdm */	{type_short_int, type_double},
		/* receive */	{0},
		/* sd */	{0},
		/* send */	{0},
		/* stdev */	{type_short_int},
		/* subarray */	{type_short_int, type_short_int, type_short_int},
		/* surv */	{type_short_int, type_short_int},
		/* szd */	{type_short_int},
		/* tag */	{type_short_int, type_short_int},
		/* tcollect */	{type_short_int, type_short_int, type_short_int, type_short_int, type_int, type_int},
		/* text */	{type_short_int, type_string},
	/* file */
		/* load */	{type_short_int, type_string, type_int},
		/* save */	{type_short_int, type_string},
	/* list */
		/* dl */	{type_short_int, type_short_int},
		/* sl */	{type_short_int},
	/* loop */
		/* lp */	{type_short_int, type_int},
		/* lpseed */	{type_short_int, type_short_int, type_int},
		/* lpseed64 */	{type_short_int, type_short_int, type_short_int, type_int},
		/* randinit */	{type_short_int, type_double},
		/* sizelin */	{type_short_int, type_int, type_int, type_int},
		/* tsc */	{type_short_int},
		/* tsc64 */	{type_short_int},
	/* plot */
		/* active */	{type_short_int, type_int, type_short_int, type_string, type_string},
		/* animate */	{type_short_int, type_short_int, type_short_int, type_short_int, type_int, type_int, type_int, type_double, type_string, type_string},
		/* animatert */	{type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_double, type_double, type_double, type_double, type_string, type_string, type_string, type_string},
		/* bregplot */	{type_short_int, type_string, type_string},
		/* bregplotm */	{type_short_int, type_string, type_string},
		/* clspins */	{type_int, type_string, type_string},
		/* spins */	{type_short_int, type_short_int, type_int, type_short_int, type_string, type_string},
		/* xy */	{type_short_int, type_short_int, type_string, type_string},
	/* run */
		/* run */	{0},
	/* save */
		/* adddata */	{type_short_int, type_short_int, type_short_int, type_int},
		/* brdim */	{type_short_int, type_short_int, type_short_int, type_short_int, type_int},
		/* chksameg */	{type_short_int, type_short_int, type_int},
		/* constean */	{type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_int, type_int},
		/* constebr */	{type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_int, type_int},
		/* fa */	{type_short_int, type_short_int, type_int},
		/* fbr */	{type_short_int, type_short_int, type_int},
		/* fcl */	{type_short_int, type_short_int, type_short_int, type_int},
		/* fe */	{type_short_int, type_short_int, type_int},
		/* fm */	{type_short_int, type_short_int, type_int},
		/* fs */	{type_short_int, type_short_int, type_int},
		/* ft */	{type_short_int, type_short_int, type_int},
		/* numbr */	{type_short_int, type_short_int, type_int},
		/* perc */	{type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_int},
		/* plotfbr */	{type_short_int, type_int},
		/* pseudobl */	{type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_short_int, type_int},
		/* svlat */	{type_short_int, type_int, type_int},
		/* svseed */	{type_short_int, type_short_int, type_int},
		/* svseed64 */	{type_short_int, type_short_int, type_short_int, type_int},
	/* set */
		/* af */	{type_short_int},
		/* altbias */	{type_short_int},
		/* bc */	{type_short_int, type_short_int, type_short_int},
		/* bias */	{type_short_int},
		/* blkinit */	{type_int},
		/* block */	{type_short_int, type_int, type_int, type_int, type_int, type_int, type_int},
		/* ev */	{type_short_int, type_short_int, type_short_int, type_short_int, type_int, type_int, type_int, type_double, type_short_int},
		/* ldinit */	{0},
		/* ldlat */	{0},
		/* randlat */	{type_double},
		/* seed */	{type_int},
		/* seed64 */	{type_double, type_double},
		/* settsc */	{0},
		/* size */	{type_int, type_int, type_int},
		/* sp */	{0},
		/* time */	{type_double},
	/* step */
		/* avgtdrop */	{type_short_int, type_short_int, type_short_int, type_int},
		/* chk */	{type_short_int},
		/* del */	{type_short_int, type_short_int},
		/* edrop */	{type_short_int, type_short_int, type_int},
		/* flips */	{type_short_int, type_short_int, type_int},
		/* force */	{type_short_int, type_double},
		/* forceend */	{type_short_int, type_short_int, type_short_int, type_double, type_int},
		/* forceenda */	{type_short_int, type_short_int, type_short_int, type_double, type_int},
		/* getcutoff */	{type_short_int, type_short_int, type_int, type_double, type_int},
		/* intfcom */	{type_short_int, type_short_int, type_int, type_double, type_short_int},
		/* latrng */	{type_short_int, type_int},
		/* longdrop */	{type_short_int, type_short_int, type_short_int, type_int},
		/* magstop */	{type_short_int, type_int},
		/* stop */	{type_short_int, type_int},
		/* stoplong */	{type_short_int, type_short_int, type_short_int, type_int},
		/* svact */	{type_short_int, type_short_int, type_int},
		/* svconste */	{type_short_int, type_short_int, type_short_int, type_short_int, type_int},
		/* sven */	{type_short_int, type_short_int, type_int},
		/* svmag */	{type_short_int, type_short_int, type_int},
		/* svtm */	{type_short_int, type_short_int, type_int},
		/* svtmdrop */	{type_short_int, type_short_int, type_int},
		/* tdropvst */	{type_short_int, type_short_int, type_short_int, type_int},
	/* exit / quit */
		/* exit */	{0},
		/* quit */	{0}
};

