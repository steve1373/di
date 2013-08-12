/********************************************************************
* This program is modified according to the original program. In this 
* modification we want to use weekly base instead of monthly base.
* Rewritten by Fangrui Ma
* July 30th, 2000
*
*       Standardized Precipitation Index.
*
*
*      Usage: spi length [length ...] [<infile] [>outfile]
*
*      Where:
*         run_length - integer > 0; running sum length for SPI
*         infile - optional input file; if omitted, spi reads
*                  from standard input.
*         outfile - optional output file; if omitted, spi writes
*                  to standard output.
*
*
*********************************************************************/

#include "spi.h"
#include <string.h>

#define NLEN 64

int Calbeg=0, Calend=9999;

int main(int argc, char *argv[])
{
  char header[80], opt[] = "-bc-ec";
  int nyrs, last, i, iopt, len[NLEN], nlen, ilen;
  double  prec[NYRS][MONTH_OR_WEEK];  /* Array of raw data */
  double beta[MONTH_OR_WEEK], gamm[MONTH_OR_WEEK], pzero[MONTH_OR_WEEK];
  double *spi[NLEN];

  if(argc < 2)  /* expect at least one length */
    {
      fprintf(stderr, 
        "Usage: spi [-bc yr] [-ec yr] length [length ...] [<in] [>out]\n");
      exit(10);
    }
/* ####
 *      -bc means "the beginning of calibration year, while
 *      -ec means "the end of calibration year.
 * ####
 */

  nlen = 0;
  for(i=1; i<argc; i++)
    {
      if(argv[i][0] == '-')
	{
	  iopt = (int)strstr(opt, argv[i]);
	  if(iopt == 0)
	    {
	      fprintf(stderr, 
		      "spi: unknown option %s\n", argv[i]);
	      exit(20);
	    }
	  iopt -= (int) opt; /* what does this mean? */
	  if(iopt == 0)
	    Calbeg = atoi(argv[++i]);
	  else
	    Calend = atoi(argv[++i]);
	}
      else /* If no calibrations specified */
	{
	  len[nlen] = atoi(argv[i]);
	  if(len[nlen] <= 0)
	    {
	      fprintf(stderr, 
		      "Usage: spi length [length ...] [<infile] [>outfile]\n");
	      fprintf(stderr, "spi: run length must be integer > 0\n");
	      exit(30);
	    }
	  nlen++; /* count the number of the length specified */ 
	  if(nlen > NLEN) /* Check for greater than max # of length */
	    {
	      fprintf(stderr, "spi: Sorry, no more than %d lengths.\n", NLEN);
	      exit(40);
	    }
	}
    }
  
  nyrs = rd_prec(header, prec, stdin); /* Get the input from input date file */
  puts(header); /* Output the header */
  printf("Calibration period=%4d-%4d", Calbeg, Calend);
  printf("   Lengths=");
  for(i=0; i<nlen; i++) printf("%3d ", len[i]);
  printf("\n");

  /* Compute SPI's */
  for(i=0; i<nlen; i++)
    {
      spi[i] = (double *)malloc(NYRS*MONTH_OR_WEEK*sizeof(double));
/* ## check the avilability of the memory */
	if (spi[i] == NULL)
	{
		printf("Not enough memory.\n");
		exit(1);
	}

#ifdef USE_EMPIRIC
      spi_empiric (len[i], (double *)prec, spi[i]);
#else
      spi_gamma (len[i], (double *)prec, beta, gamm, pzero, spi[i]);
#endif
    }

  /* Skip leading MISSINGs */
  for(i=0; i<NYRS*MONTH_OR_WEEK; i++)
    if (spi[0][i] != MISSING) break;

  /* Skip trailing MISSINGs */
  for(last=NYRS*MONTH_OR_WEEK-1; last>0; last--)
    if (spi[0][last] != MISSING) break;
  last++;

  for(; i<last; i++)
    {
      printf("%4d %2d ",
	     i/MONTH_OR_WEEK+BEGYR, i%MONTH_OR_WEEK+1);
      for(ilen=0; ilen<nlen; ilen++)
	{
	  printf("%6.2f ", spi[ilen][i]);
	}
      printf("\n");
    }
  exit(0);
}
