from make2FGLxml import *
import sys

name=sys.argv[1]
ra=sys.argv[2]
dec=sys.argv[3]

mymodel=srcList('gll_psc_v04.fit','%s_gtselect.fits' % name,'%s_fermisources_model.xml' %name)
mymodel.makeModel('gll_iem_v05_rev1.fit','gll_iem_v05_rev1','iso_source_v05_rev1.txt','iso_source_v05')
                                                
out="%s_input_model.xml" %name
outputFile=open(out, 'w')

with open('%s_fermisources_model.xml'% name,'r') as infile:
    for line in infile:
        if line.startswith('</source_library>')==False:
            outputFile.write(line)
            
outputFile.write('\n\
<!-- My sources -->\n\
<source name="%s" type="PointSource">\n\
<spectrum type="PowerLaw">\n\
<parameter free="1" max="1000.0" min="0.001" name="Prefactor" scale="1e-09" value="10"/>\n\
<parameter free="1" max="-1.0" min="-5.0" name="Index" scale="1.0" value="-2.1"/>\n\
<parameter free="0" max="2000.0" min="30.0" name="Scale" scale="1.0" value="100.0"/>\n\
</spectrum>\n\
<spatialModel type="SkyDirFunction">\n\
<parameter free="0" max="360" min="-360" name="RA" scale="1.0" value="%s"/>\n\
<parameter free="0" max="90" min="-90" name="DEC" scale="1.0" value="%s"/>\n\
</spatialModel>\n\
</source>\n\
</source_library>\n' % (name,ra,dec))

outputFile.close()
