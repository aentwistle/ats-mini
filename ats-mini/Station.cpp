#include "Common.h"
#include "Menu.h"

// CB frequency range
#define MIN_CB_FREQUENCY 26060
#define MAX_CB_FREQUENCY 29665

//
// Named frequencies, such as CB channels, etc
//
static const NamedFreq namedFrequencies[] =
{
  { 26965, "CH1" },  { 26975, "CH2" },  { 26985, "CH3" },  { 27005, "CH4" },
  { 27015, "CH5" },  { 27025, "CH6" },  { 27035, "CH7" },  { 27055, "CH8" },
  { 27065, "CH9" },  { 27075, "CH10" }, { 27085, "CH11" }, { 27105, "CH12" },
  { 27115, "CH13" }, { 27125, "CH14" }, { 27135, "CH15" }, { 27155, "CH16" },
  { 27165, "CH17" }, { 27175, "CH18" }, { 27185, "CH19" }, { 27205, "CH20" },
  { 27215, "CH21" }, { 27225, "CH22" }, { 27235, "CH24" }, { 27245, "CH25" },
  { 27255, "CH23" }, { 27265, "CH26" }, { 27275, "CH27" }, { 27285, "CH28" },
  { 27295, "CH29" }, { 27305, "CH30" }, { 27315, "CH31" }, { 27325, "CH32" },
  { 27335, "CH33" }, { 27345, "CH34" }, { 27355, "CH35" }, { 27365, "CH36" },
  { 27375, "CH37" }, { 27385, "CH38" }, { 27395, "CH39" }, { 27405, "CH40" },
};

char bufStationName[50] = "";
char bufMessage[100]    = "";

const char *getStationName()
{
#ifdef THEME_EDITOR
  return("*STATION*");
#else
  return(bufStationName);
#endif
}

void clearStationName()
{
  bufStationName[0] = '\0';
}

static bool showRdsStation(const char *stationName)
{
  if(stationName && strcmp(bufStationName, stationName))
  {
    strcpy(bufStationName, stationName);
    return(true);
  }

  return(false);
}

#if 0 // Not used yet, enable later
static bool showRdsMessage(const char *rdsMessage)
{
  if(rdsMessage && strcmp(bufMessage, rdsMessage))
  {
    strcpy(bufMessage, rdsMessage);
    return(true);
  }

  return(false);
}
#endif

static bool showRdsTime(const char *rdsTime)
{
  if(!rdsTime) return(false);

  // The standard RDS time format is “HH:MM”.
  // or sometimes more complex like “DD.MM.YY,HH:MM”.
  const char *timeField = strstr(rdsTime, ":");

  // If we find a valid time format...
  if(timeField && (timeField>=rdsTime+2) && timeField[1] && timeField[2])
  {
    // Extract hours and minutes
    int hours = (timeField[-2] - '0') * 10 + timeField[-1] - '0';
    int mins  = (timeField[1] - '0') * 10 + timeField[2] - '0';

    // If hours and minutes are valid, update clock
    if(hours>=0 && hours<24 && mins>=0 && mins<60)
      return(clockSet(hours, mins));
  }

  // No time
  return(false);
}

bool checkRds()
{
  bool needRedraw = false;

  rx.getRdsStatus();

  if(rx.getRdsReceived() && rx.getRdsSync() && rx.getRdsSyncFound())
  {
    needRedraw |= showRdsStation(rx.getRdsText0A());
//    needRedraw |= showRdsMessage(rx.getRdsText2A());
    needRedraw |= (getRDSMode() & RDS_CT) && showRdsTime(rx.getRdsTime());
  }

  // Return TRUE if any RDS information changes
  return(needRedraw);
}

static const char *findNameByFreq(uint16_t freq, const NamedFreq *db, uint16_t dbSize)
{
  int r, l;

  for(l=0, r=dbSize-1 ; l <= r ; )
  {
    int m = (l + r) >> 1;
    if(db[m].freq < freq)      l = m + 1;
    else if(db[m].freq > freq) r = m - 1;
    else return(db[m].name);
  }

  return(0);
}

bool checkFreqName(uint16_t freq)
{
  const char *name = findNameByFreq(freq, namedFrequencies, ITEM_COUNT(namedFrequencies));
  return(showRdsStation(name? name : ""));
}
