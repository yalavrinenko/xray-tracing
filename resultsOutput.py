#coding: utf-8
'''
Created on 02.04.2015

@author: cheshire
'''

import math
import numpy
from fileinput import filename

def mstr(val):    
    try:
        basestring
    except NameError:
        basestring = str
        
    if (isinstance(val, basestring)):
        return val
    if (type(val) == int):
        return "%d" % val
    return "%0.6g" % val

def readFile(file, x, y, z, colors):
    f = open(file, 'r')
    
    f.readline()
    
    phiLine = f.readline()
    phils = phiLine.split()
    
    phi=float(phils[2])
    
    l = f.readline()
    
    coords = l.split()
        
    ctmpx = coords[2][1:-1]
    ctmpy = coords[3][:-1]
    ctmpz = coords[4][:-2]
    
    cx = float(ctmpx)
    cy = float(ctmpy)
    cz = float(ctmpz)
    
    f.readline()
    
    coords = l.split()
        
    ctmpx = coords[2][1:-1]
    ctmpy = coords[3][:-1]
    ctmpz = coords[4][:-2]
    
    ncx = float(ctmpx)
    ncy = float(ctmpy)
    ncz = float(ctmpz)
    
    phi *= math.pi / 180.0
    phi = (math.pi / 2.0 - phi)
    
    if ncx > 0:
        phi=-phi
    
    f.readline()
    f.readline()
        
    lines = f.readlines()    
    for i in lines:
        words = i.split()
        wx = float(words[0]) - cx
        wy = float(words[1]) - cy
        wz = float(words[2]) - cz
        c = float(words[3])
        
        dwx = wx * math.cos(phi) - wy * math.sin(phi)
        dwy = wx * math.sin(phi) + wy * math.cos(phi)
        
        wx = dwx
        wy = dwy
        
        x.append(wx)
        y.append(wy)
        z.append(wz)
        colors.append(c)


    f.close()        
    print (file,len(x))

class resPlot():

    wList = {}
    reflList = {}
    pixel_size = 0.0001

    def __init__(self, fAx,fFf, mAx,mFf, rAx,rFf,wrAx,xrA,xrF,oB):
        self.filmAxix=fAx
        self.filmFig=fFf
        
        self.mirrorAxix=mAx
        self.mirrorFig=mFf
        
        self.reflAxix=rAx
        self.reflFig=rFf
        
        self.wResAxix = wrAx
        
        self.xResAxix = xrA
        self.xResFig = xrF
        
        self.outBuf=oB
    
    def shrDataToZeroWave(self,x,w,dWave,zeroWave):
        coord = 0
        np = 0
        for i,c in enumerate(x):
            if (zeroWave - dWave <= w[i] <= zeroWave + dWave):
                coord += x[i]
                np += 1
        
        isExeption = 0
        
        try:
            coord /= np
        except ZeroDivisionError:
            isExeption = 1
            coord = 0.0
            
        for i, c in enumerate(x):
            x[i] -= coord
            
        return x
    
    def getCoeff(self,x,w,dWave,zeroWave):
        coord = 0
        np = 0
        for i,c in enumerate(x):
            if (zeroWave - dWave <= w[i] <= zeroWave + dWave):
                coord += x[i]
                np += 1
        
        isExeption = 0
        
        try:
            coord /= np
        except ZeroDivisionError:
            isExeption = 1
            coord = 0.0
                
        for i, c in enumerate(x):
            x[i] -= coord
                
        if (len(x) == 0):
            return [],[]
        
        res = numpy.polyfit(x, w, 2)
        data=""
        data+="\n\n[RESULTS FOR REF.  "+ u'\u03BB'+"=" + mstr(zeroWave) + "A ]\n"
        
        self.dispCurve[int(zeroWave*1e+6)]="Exeption" if isExeption else " "+ u'\u03BB'+" = "+mstr(res[0])+"X**2 + "+mstr(res[1])+"X +"+mstr(res[2])
        
        data+=self.dispCurve[int(zeroWave*1e+6)]
        
        self.outBuf.insert(self.outBuf.get_end_iter(),data)
        
        if (round(res[0],8) == 0.0 and round(res[1],8) == 0 ):
            wspace = w
        else:
            wspace = x
            for i, c in enumerate(wspace):
                wspace[i]=res[0]*c**2 + res[1]*c + res[2]
        
        return res, wspace
    
    def changeDetectorAxixLimits(self, x,y):
        self.filmAxix.set_xlim(x[0],x[1])
        self.filmAxix.set_ylim(y[0],y[1])
        self.filmFig.canvas.draw()
    
    def plotWaveLengthUp(self,wl, limits = []):        
        mrefl = -1
        for key in self.wList.iterkeys():
            mrefl = max(max(self.reflList[key]),mrefl)
        
        if (mrefl < 0):
            mrefl=1
        
        hl=[mrefl for i in wl]
        self.reflAxix.bar(wl,hl,0.0001)
        
        if (limits != []):
            self.reflAxix.set_xlim([min(limits)*0.95, max(limits)*1.05])
            hlimits = [mrefl for i in limits]
            self.reflAxix.bar(limits,hlimits,0.006, color='red', edgecolor='red')
        
        for key in self.wList.iterkeys():
            self.reflAxix.plot(self.wList[key],self.reflList[key],u'o')
            
        self.reflFig.canvas.draw()  
    
    def plotWaveLength(self,wl, limits = []):
        
        self.reflAxix.clear()
        
        mrefl = -1
        for key in self.wList.iterkeys():
            mrefl = max(max(self.reflList[key]),mrefl)
        
        if (mrefl < 0):
            mrefl=1
        
        hl=[mrefl for i in wl]
        self.reflAxix.bar(wl,hl,0.0001)
        
        if (limits != []):
            self.reflAxix.set_xlim([min(limits)*0.95, max(limits)*1.05])
            hlimits = [mrefl for i in limits]
            self.reflAxix.bar(limits,hlimits,0.006, color='red', edgecolor='red')
        
        for key in self.wList.iterkeys():
            self.reflAxix.plot(self.wList[key],self.reflList[key],u'o')
        
        self.reflFig.canvas.draw()    
    
    def testPlot(self):
        x=range(1,100)
        y=[math.sin(z) for z in x]
        
        self.filmAxix.plot(x,y)
        self.mirrorAxix.plot(x,y)
        self.reflAxix.plot(x,y)
        
        self.filmFig.canvas.draw()
        self.reflFig.canvas.draw()
        self.mirrorFig.canvas.draw()
        
    def plotReflResults(self,fileName):
        pattern="[lambda gen catch relf I]="
        ToRet = []
        f = open(fileName,"r")
        
        self.wList[fileName]=[]
        self.reflList[fileName]=[]
        
        for l in f:
            if (pattern in l):
                s=l[len(pattern)+1:-1].split()
                self.wList[fileName] += [float(s[0])]
                if (float(s[2]) == 0):
                    s[2] = "1.0"
                self.reflList[fileName] += [float(s[3]) / float(s[2]) *100.0]
                ToRet += [[float(s[0]), float(s[2]), float(s[3]), float(s[3]) / float(s[2])]]
        
        self.reflAxix.plot(self.wList[fileName],self.reflList[fileName],u'o')
        self.reflFig.canvas.draw()
        
        return ToRet
                
    def ClearAll(self):
        
        self.wList={}
        self.reflList={}
        
        self.reflAxix.clear()
        self.filmAxix.clear()
        self.mirrorAxix.clear()
        self.wResAxix.clear()
        self.xResAxix.clear()
        
        self.mirrorAxix.set_xlabel("x, [mm]",labelpad=-33)
        self.mirrorAxix.set_ylabel("y, [mm]")        
               
        self.filmAxix.set_xlabel("x, [mm]",labelpad=-33)
        self.filmAxix.set_ylabel("y, [mm]")        
        
        self.reflAxix.set_xlabel("Wavelength, [A]",labelpad=-33)
        self.reflAxix.set_ylabel("Reflectivity, %")        
        
        self.wResAxix.set_xlabel("Wavelength, [A]",labelpad=-33)
        self.wResAxix.set_ylabel("Counts, [n]")        
        
        self.xResAxix.set_xlabel("x, [mm]",labelpad=-33)
        self.xResAxix.set_ylabel("Counts, [n]")        
        
    def getWaveImageProp(self,wave,dwave,x,y,c,coef):
        group = []
        groupY = []
        groupw = []
        for i,c in enumerate(c):
            if (math.fabs(c - wave) <=dwave+1e-5):
                group += [x[i]]
                groupY += [y[i]]
                groupw += [coef[0]*x[i]**2 + coef[1]*x[i] + coef[2]]
                        
        coord = numpy.mean(group)
        FWHM = numpy.std(groupw)*2.355
        ImSize = 0.0
        if (not len(groupY) == 0):
            ImSize = numpy.max(groupY) - numpy.min(groupY)
            
        #if (not len(group) == 0):
        #    ImSize = numpy.max(group) - numpy.min(group)
        
        return [wave, round(coord,6),round(ImSize,6),round(FWHM,6)]
        
                    
    
    def analizeFilm(self,file,waves,dwaves,zwave):
        x = []
        y = []
        z = []
        colors = []
        readFile(file, x, y, z,colors)
                
        self.dispCurve={}
        coeff=[]
        for zw in waves:
            cf,wspace = self.getCoeff(list(x), colors, 0.0001, zw)
            if (not len(wspace) == 0):
                coeff=list(cf)
        
        toRet = []
        
        self.shrDataToZeroWave(x, colors, 0.0001, zwave)
        for i,wave in enumerate(waves):
            toRet += [self.getWaveImageProp(wave, dwaves[i] * 1e-3, x, z, colors, coeff)]
            
        return toRet
        
    
    def normalizeWl(self,mainOrder, targetOrder, waves, cr2d):
        
        scalef = cr2d[int(mainOrder)-1] * int(targetOrder) / (int(mainOrder) * cr2d[int(targetOrder)-1])
        
        for i,wave in enumerate(waves):
            waves[i] = scalef * wave
        return waves
    
    def plotFilm(self,files,zeroWave,wl,cr2d, dispOrders, mainOrder = 1):
        
        self.wResAxix.clear()
        self.xResAxix.clear()
        
        self.wResAxix.set_xlabel("Wavelength, [A]",labelpad=-33)
        self.wResAxix.set_ylabel("Counts, [n]")        
        self.xResAxix.set_xlabel("x, [mm]",labelpad=-33)
        self.xResAxix.set_ylabel("Counts, [n]")    
                
        for i,file in enumerate(files):
            if len(file) >= 1:
                file = file[0]
            else:
                break
            x = []
            y = []
            z = []
            colors = []
            readFile(file, x, y, z,colors)
            
            targetOrder = dispOrders[i]
            
            colors = self.normalizeWl(mainOrder, targetOrder, colors, cr2d)
            try:
                max_nbins_x = abs(abs(max(x)) - abs(min(x))) / self.pixel_size + 1
            except ValueError:
                max_nbins_x = 100
                print ("Max value error. NBINS is incorrect\n")

            H, xe, ze = numpy.histogram2d(x, z, bins=(max_nbins_x, max_nbins_x))
            xv, zv = numpy.meshgrid(xe, ze)

            self.filmAxix.pcolormesh(xv, zv, numpy.transpose(H), cmap = 'Greys')

            self.filmFig.canvas.draw()
        
            #pltx=list(x)
            pltx = [-tx for tx in x]
                    
            try:
                pltx = self.shrDataToZeroWave(pltx, colors, 0.0001, zeroWave)
                self.xResAxix.hist(pltx, orientation='vertical', bins=1000, histtype='step', label = ["O"+str(dispOrders[i])])
                self.xResAxix.legend()                
            except IndexError:
                print ("No detector data for line ", zeroWave)
            except ValueError:
                print ("No detector data for line ", zeroWave)
                
            self.dispCurve={}
            coeff,wspace = self.getCoeff(x, colors, 0.0001, zeroWave)
            if (not len(wspace) == 0):
                self.wResAxix.plot(wspace, numpy.ones(len(wspace)), color="white", alpha = 0.0)
                
            self.xResFig.canvas.draw()
       
    
    def plotMirror(self, fileName, W, H):       
            f = open(fileName, 'r')
            
            f.readline()
            f.readline()
            f.readline()
            f.readline()
            f.readline()
            f.readline()
                
            lines = f.readlines() 
            x=[]
            y=[]
            z=[]
            colors=[]   
            for i in lines:
                words = i.split()
                wx = float(words[0]) 
                wy = float(words[1])
                wz = float(words[2])
                c = float(words[3])
                
                x.append(wx)
                y.append(wy)
                z.append(wz)
                colors.append(c)
                
            print( fileName,len(x) )
            
            self.mirrorAxix.set_xlim(-W/2.0, W/2.0)
            self.mirrorAxix.set_ylim(-H/2.0, H/2.0)
            
            CAX=self.mirrorAxix.scatter(x, z, marker=',', c=colors)
            #self.mirrorFig.colorbar(CAX,orientation='horizontal')
            self.mirrorFig.canvas.draw()
