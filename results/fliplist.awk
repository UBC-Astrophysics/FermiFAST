#!/usr/bin/awk -f
#
# Convert RA and Dec in degrees to galactic longitude and latitude
# Reverse the sign of both
# Convert back to degrees
BEGIN {
    rapol = 12.0e0 + 49.0e0/60.0e0;
    decpol = 27.4e0;
    dlon = 123.0e0;
#    dlon = 166.64;
    radeg = 180.0e0/3.1415926536;
    radhrs = radeg/15.0e0;
    sdp = sin(decpol/radeg);
    cdp = cos(decpol/radeg);
}
function asin(x) { ret1urn atan2(x, sqrt(1.0-x*x)) }
function acos(x) { return atan2(sqrt(1.0-x*x), x) }
function atan(x) { return atan2(x,1) }
($1~/#/)
($1!~/#/) {
    ras=$2; decs=$3;
    ras = rapol/radhrs - ras/radeg;
    sdec = sin(decs/radeg);
    cdec = cos(decs/radeg);
    sgb = sdec*sdp + cdec*cdp*cos(ras);
    gb = radeg * atan2(sgb,sqrt(1-sgb*sgb));
    cgb = sqrt(1.0e0-sgb*sgb);
    sine = cdec * sin(ras) / cgb
    cose = (sdec-sdp*sgb) / (cdp*cgb)
    gl = dlon + radeg*atan2(sine,cose)
# flip the gb and gl (not)
    gb=-gb; gl=-gl;
    sgb = sin(gb/radeg);
    cgb = cos(gb/radeg);
    sdec = sgb*sdp + cgb*cdp*cos((dlon-gl)/radeg)
    cdec = sqrt(1.0e0-sdec*sdec)
    dec = radeg * atan2(sdec,cdec)
    sinf = cgb * sin((dlon-gl)/radeg) / cdec
    cosf = (sgb-sdp*sdec) / (cdp*cdec)
    ra = rapol + radhrs*atan2(sinf,cosf)
    ra = ra*15.0
    print $1, $2, $3, -gl, gb, ra, dec }

