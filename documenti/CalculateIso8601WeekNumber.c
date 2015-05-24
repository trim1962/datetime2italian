/***************************************************************************/
int CalculateISOweekNum (void)

{
/*
	Calcolates week number acording to ISO 8601
	Procedure by Rick McCarty, 1999
	Adapted to CVI by R.Bozzolo, 2006

	Requires #include <windows.h>

	Input: None

	Output: Number of week in the year for local current date



    Algorithm for Converting Gregorian Dates to ISO 8601 Week Date
                            (Y2K  Compliant)
                           Rick McCarty, 1999

                     From: Gregorian Year-Month-Day
                 To: ISO YearNumber-WeekNumber-Weekday 

ISO 8601 specifies that Week 01 of the year is the week containing
the first Thursday; Monday is Weekday 1, Sunday is Weekday 7;
WeekNumber requires two digits (W01, W02, etc.; "W" is optional)

Algorithm Conventions:
   "/" = integer division, discard remainder  (5/2 = 2)
   "%" = modulus, keep only remainder    (5%2 = 1)
   "&" = concatenation   ("W" & 12 = "W12")
   "!=" = unequal   (7 != 8  is true)
   "+=" = add right value to left variable,
             if F = 3, then (F += 4) yields F = 7
   "-=" = subtract right value from left variable

1. Convert input to Y M D
   Y = Year     (full specification; input 98 = year 0098)
		(Y must be larger than -1)
   M = Month    (1 through 12)
   D = Day      (1 through 31)
2. Find if Y is LeapYear
   if (Y % 4 = 0  and  Y % 100 != 0) or Y % 400 = 0
      then
         Y is LeapYear
      else
         Y is not LeapYear
3. Find if Y-1 is LeapYear
4. Find the DayOfYearNumber for Y M D
   Mnth[1] = 0    Mnth[4] = 90    Mnth[7] = 181   Mnth[10] = 273
   Mnth[2] = 31   Mnth[5] = 120   Mnth[8] = 212   Mnth[11] = 304
   Mnth[3] = 59   Mnth[6] = 151   Mnth[9] = 243   Mnth[12] = 334
   DayOfYearNumber = D + Mnth[M]
   if Y is LeapYear and M > 2
      then
         DayOfYearNumber += 1
5. Find the Jan1Weekday for Y (Monday=1, Sunday=7)
   YY = (Y-1) % 100
   C = (Y-1) - YY
   G = YY + YY/4
   Jan1Weekday = 1 + (((((C / 100) % 4) x 5) + G) % 7)
6. Find the Weekday for Y M D
   H = DayOfYearNumber + (Jan1Weekday - 1)
   Weekday = 1 + ((H -1) % 7)
7. Find if Y M D falls in YearNumber Y-1, WeekNumber 52 or 53
   if DayOfYearNumber <= (8-Jan1Weekday) and Jan1Weekday > 4
      then
         YearNumber = Y - 1 
         if Jan1Weekday = 5 or (Jan1Weekday = 6 and Y-1 is LeapYear)
            then
               WeekNumber = 53
            else 
               WeekNumber = 52
      else 
         YearNumber = Y
8. Find if Y M D falls in YearNumber Y+1, WeekNumber 1
   if YearNumber = Y
      then
         if Y is LeapYear
            then 
               I = 366
               else
               I = 365
         if (I - DayOfYearNumber) < (4 - Weekday)
            then
               YearNumber = Y + 1
               WeekNumber = 1
9. Find if Y M D falls in YearNumber Y, WeekNumber 1 through 53
   if YearNumber = Y
      then
         J = DayOfYearNumber + (7 - Weekday) + (Jan1Weekday -1)
         WeekNumber = J / 7
         if Jan1Weekday > 4
               WeekNumber -= 1
10. Output ISO Week Date:
   if WeekNumber < 10
      then
         WeekNumber = "0" & WeekNumber  (WeekNumber requires 2 digits)
   Print: YearNumber & "-" & WeekNumber & "-" & Weekday    (Optional: "-W" & WeekNumber)
*/

	int		YY, C, G, I, J, DayOfYearNumber, Jan1Weekday, Weekday, YearNumber;
	int		LeapYear = 0, precLeapYear = 0, WeekNumber = 0;
	int		Mnth[] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	SYSTEMTIME	dh;

	//1. Convert input to Y M D
	//2. Find if Y is LeapYear
	GetLocalTime(&dh);
	if ((dh.wYear % 4 == 0 && dh.wYear % 100 != 0) || dh.wYear % 400 == 0)
		LeapYear = 1;

	//3. Find if Y-1 is LeapYear
	if (((dh.wYear - 1) % 4 == 0 && (dh.wYear - 1) % 100 != 0) || (dh.wYear % 400 - 1) == 0)
		precLeapYear = 1;

	//4. Find the DayOfYearNumber for Y M D
	DayOfYearNumber = dh.wDay + Mnth[dh.wMonth];
	if (LeapYear && dh.wMonth > 2) DayOfYearNumber++;

	//5. Find the Jan1Weekday for Y (Monday=1, Sunday=7)
	YY = (dh.wYear - 1) % 100;
	C = (dh.wYear - 1) - YY;
	G = YY + YY / 4;
	Jan1Weekday = 1 + (((((C / 100) % 4) * 5) + G) % 7);

	//6. Find the Weekday for Y M D
	Weekday = dh.wDayOfWeek;
	if (!Weekday) Weekday = 7;		// Gestisce correttamente la domenica (0 in SYSTEMTIME)

	//7. Find if Y M D falls in YearNumber Y-1, WeekNumber 52 or 53
	if (DayOfYearNumber <= (8 - Jan1Weekday) && Jan1Weekday > 4) {
		YearNumber = dh.wYear - 1;
		if (Jan1Weekday == 5 || (Jan1Weekday == 6 && precLeapYear))
			WeekNumber = 53;
		else 
			WeekNumber = 52;
	}
	else 
		YearNumber = dh.wYear;

	//8. Find if Y M D falls in YearNumber Y+1, WeekNumber 1
	if (YearNumber == dh.wYear) {
		if (LeapYear) I = 366; else I = 365;
		if ((I - DayOfYearNumber) < (4 - Weekday)) {
			YearNumber = dh.wYear + 1;
			WeekNumber = 1;
		}
	}

	//9. Find if Y M D falls in YearNumber Y, WeekNumber 1 through 53
	if (YearNumber == dh.wYear) {
		J = DayOfYearNumber + (7 - Weekday) + (Jan1Weekday -1);
		WeekNumber = J / 7;
		if (Jan1Weekday > 4) WeekNumber -= 1;
	}

	return WeekNumber;
}

