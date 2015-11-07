
/* array values for effective area */

#define AEFF_ENERG_LO 0
#define AEFF_ENERG_HI 1
#define AEFF_CTHETA_LO 2
#define AEFF_CTHETA_HI 3
#define AEFF_EFF_AREA 4
#define NAEFF_DATA 5

typedef 
struct aeff_data_struct {
  float ***aeffdata, mustep, lestep, mumin, lemin;
  int nE, nMu;
  char units[80];
} AEFF_DATA_STRUCT;

void freeeffarea(AEFF_DATA_STRUCT *aeffds);
int loadeffarea(char *passfile, AEFF_DATA_STRUCT *aeffds);

