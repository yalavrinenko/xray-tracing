import math
import os
import glob
import sys
import numpy as np
import numpy
import threading
from ctypes import *
import locale
import time
if sys.platform.startswith('linux'):
    from gi.repository import Gtk
else:
    import gtk as Gtk

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

def genSetOfWave(wMin,wMax,depth,maxDepth,depthLimit):
    wCentral=round((wMin+wMax)*0.5,6)
    
    if (depth == maxDepth):
        return [wCentral]
    
    if (depth >= depthLimit):
        return []
    
    Result = [wCentral]
    #go rigth
    Result += genSetOfWave(wCentral, wMax, depth+1, maxDepth, depthLimit)
    #go left
    Result += genSetOfWave(wMin, wCentral, depth+1, maxDepth+1, depthLimit)
    
    return Result
        

class vect:
    def __init__(self,_x,_y):
        self.x = _x
        self.y = _y
    
    def norm(self):
        vlen=(self.x**2.0+self.y**2.0)**0.5
        self.x /= vlen
        self.y /= vlen
        
    def dot(self,b):
        return self.x*b.x+self.y*b.y
    
    def angl(self,b):
        self.norm()
        b.norm()
        vmul = self.dot(b)
        return math.acos(vmul)
    
    def build(self,b):
        return vect(self.x-b.x,self.y-b.y)

class systemConfig:
    crystal2d = {}
    crystalR = 150
    centralWave = 2.0    
    BraggA = 0.0
    SrcDist = 0.0
    DstDist = 0.0
    CrystType = "Spherical"     #$MrType = "Sphere"  ;"Sphere" ;Cylinder
    
    crystalW = 50
    crystalH = 40
    
    SrcSizeW = 0.1
    SrcSizeH = 0.1
    
    SrcCone = 0.0
    
    RayCount = 10000
    RayByIter = 1000
    
    FilmDistFromCenter = 0.0
    ToFilmDirection = 0.0
    FilmAngle = 0.0
    FilmSizeW = 400
    FilmSizeH = 400 
    
    DistrFileName = {}
    DistrHW = {}
    DistrStep = {}
    DistrSize = {}
    Orders = []
    resPlot = None
    mainOrder = 1

    SlitPos = 0.0
    SlitW = 0.0
    SlitH = 0.0
    
    WaveLengths = []
    WaveIntensity = []
    dWaveLength = []
    WaveLimits = []
    WaveAutoscale = False
    
    
    zeroWave = 0.0
    
    isCompute = 0


    progressInfoOut = None  
    exitThreadFlags = False
    exitMainCompThread = False

    def crystClear(self):
        print	("Cryst. Par. clear")
        self.DistrFileName = {}
        self.DistrHW = {}
        self.DistrStep = {}
        self.DistrSize = {}
        self.crystal2d = {}

    
    def calcLimWaveLength(self,order):
        
        phi = self.BraggA * np.pi / 180.0
        L,R,H= [self.SrcDist, self.crystalR, self.crystalW/2.0]
        
        CR = np.array([0.0,L * np.sin(phi)  -R])
        SR = np.array([-L * np.cos(phi), 0])
        
        S = CR[1] + (R**2.0 - H**2.0)**0.5
        
        waves = []
        
        for mult in [-1.0, 1.0]:
            CrossR = np.array([mult*H, S])
            r1 = SR - CrossR
            r2 = CR - CrossR
            cosT = np.dot(r1,r2) / (np.dot(r1,r1)**0.5 * np.dot(r2,r2)**0.5)
            Theta = np.pi/2.0 - np.arccos(cosT)
            P = self.crystal2d[order - 1] * np.sin(Theta)
            waves += [P]        
        
        return [np.max(waves), np.min(waves)]

    
    def dumpPatter(self, oFile):
        iFile = open("sys/pattern.par","r")
        lines=iFile.readlines();
        oFile.writelines(lines)
    
    def prepareComputation(self):
        parFiles = []
        for idx in self.Orders:
            [w1,w2]=self.calcLimWaveLength(idx)
            
            self.WaveLengths += [w2] 
            self.WaveIntensity += [0.0]
            self.dWaveLength += [w2]
                 
            self.WaveLengths += [w1]
            self.WaveIntensity += [0.0]
            self.dWaveLength += [w1]
            
            fileName = "par/Order_" + mstr(idx)+".par"
            parFiles += [fileName]
            outFile = open(fileName,"w+")
            
            idx -= 1
            outFile.write("$GenName = " + "Order_"+mstr(idx+1) + "\n")
            outFile.write("$rayCnt = " + mstr(self.RayCount) + "\n")
            outFile.write("$rayByIt = " + mstr(self.RayByIter) + "\n")
            outFile.write("$cR = "+ mstr(self.crystalR) + "\n")
            outFile.write("$wA = "+ mstr(self.crystalW) + "\n")
            outFile.write("$hA = "+ mstr(self.crystalH) + "\n")
            outFile.write("$ScattAngle = "+ mstr(self.BraggA) + "\n")
            outFile.write("$Cr2D = "+ mstr(self.crystal2d[idx]) + "\n")
            outFile.write("$DistrWidth = "+ mstr(self.DistrHW[idx]) + "\n")
            outFile.write("$RFile = "+ self.DistrFileName[idx] + "\n")
            outFile.write("$RSize = "+ mstr(self.DistrSize[idx])+ "\n")
            outFile.write("$RStep = "+ mstr(self.DistrStep[idx])+ "\n")
            outFile.write("$SrcDist = "+ mstr(self.SrcDist)+ "\n")
            outFile.write("$SrcCone = "+ mstr(self.SrcCone)+ "\n")
            outFile.write("$DDist  = "+ mstr(self.DstDist)+ "\n")
            outFile.write("$FilmA  = "+ mstr(self.FilmAngle)+ "\n")
            outFile.write("$FildDir = "+ mstr(self.ToFilmDirection) + "\n")
            outFile.write("$WLCount  = "+ mstr(len(self.WaveLengths) + 2)+ "\n")
            
            outFile.write("$SrcSizeW = " + mstr(self.SrcSizeW/2.0) + "\n")
            outFile.write("$SrcSizeH = " + mstr(self.SrcSizeH/2.0) + "\n")
            outFile.write("$FilmH = " + mstr(self.FilmSizeH) + "\n")
            outFile.write("$FilmW = " + mstr(self.FilmSizeW) + "\n")
            outFile.write("$SlitPos = " + mstr(self.SlitPos) + "\n")
            outFile.write("$SlitW = " + mstr(self.SlitW) + "\n")
            outFile.write("$SlitH = " + mstr(self.SlitH) + "\n")
            
            #MrType = "Sphere" 
            #if self.CrystType == "Spherical":
            #    MrType = "Cylinder"
            MrType=self.CrystType
            
            outFile.write("$MrType = " + MrType + "\n")
            
            outFile.write("\n\n\n")
            
            self.dumpPatter(outFile)
            
            waveIndex = 1
            for i, w in enumerate(self.WaveLengths):
                if (w2*0.95 <= w <= w1*1.05):
                    outFile.write("wave"+mstr(waveIndex)+" = "+mstr(w)+"\n")
                    outFile.write("dwave"+mstr(waveIndex)+" = "+mstr(self.dWaveLength[i] * 1e-3)+"\n")
                    outFile.write("iwave"+mstr(waveIndex)+" = "+mstr(self.WaveIntensity[i])+"\n")
                    waveIndex += 1
                                    
            outFile.close();
            
        return parFiles
    
    def __init__(self, logWindow):
        self.outText = logWindow
        if sys.platform.startswith('linux'):
            self.rayTraceLib = CDLL("./cpp/build/libRaytrace.so")
            print ('OS Linux')
        else:
            try:
                self.rayTraceLib = CDLL("./cpp/win-x86_64/libRaytrace.dll")
                print("Use x64 version.\n")
            except WindowsError:
                self.rayTraceLib = CDLL("./cpp/win-x86/libRaytrace.dll")
                print("Use x86 version.\n")
            print ('OS Windows')

    def free_lib(self):
        if (not sys.platform.startswith('linux')):
            libHandle = self.rayTraceLib._handle
            del self.rayTraceLib
            windll.kernel32.FreeLibrary(libHandle)
    
    def get_srcDist(self):
        return mstr(self.SrcDist)
    
    def get_bragg(self):
        return mstr(self.BraggA)
    
    def dispData(self):
        data = ""
        data += "Ray count / ray per iteration " + mstr(self.RayCount) + "/" + mstr(self.RayByIter) + "\n"
        data += "Crystal type = " + self.CrystType + "\n"
        data += "Crystal R = " + mstr(self.crystalR) + " [mm]\n"
        data += "Scattering Angle = " + mstr(self.BraggA) + " [deg]\n"
        data += "Source distance = " + mstr(self.SrcDist) + " [mm]\n"
        data += "Detector distance = " + mstr(self.DstDist) + " [mm]\n"
        data += "Central wavelength = " + mstr(self.centralWave) + " [A]\n"
        data += "Detector distance from center = " + mstr(self.FilmDistFromCenter) + " [mm]\n"
        data += "Detector direction from center = " + mstr(self.ToFilmDirection) + " [deg]\n"
        data += "Detector size W x H= " + mstr(self.FilmSizeW) + " X " + mstr(self.FilmSizeH) + " [mm]\n"
        data += "Detector angle = " + mstr(self.FilmAngle) + " [deg]\n"
        data += "Crystal W x H = " + mstr(self.crystalW) + " X " + mstr(self.crystalH) + " [mm]\n"
        data += "Source cone = " + mstr(self.SrcCone) + " [deg]\n"
        data += "Source size W x H= " + mstr(self.SrcSizeW) +" X "+ mstr(self.SrcSizeH) + " [mm]\n"
        
        if (not self.SlitPos == 0):
            data += "Slit distance = " + mstr(self.SlitPos) + " [mm]\n"
            data += "Slit size W x H= " + mstr(self.SlitW) + " X " + mstr(self.SlitH) + " [mm]\n"
        
        
        for i in self.DistrSize:
            if (self.DistrSize[i] != 0):
                data += "\n\n[REFL. FUNCTION IN " + mstr(i + 1 ) + " ORDER]\n\n"
                data += "Crystal 2d = " + mstr(self.crystal2d[i]) + " [A]\n"
                data += "REFL. File Name = " + self.DistrFileName[i] + "\n"
                data += "REFL. HW = " + mstr(self.DistrHW[i]) + " [deg]\n"
                data += "REFL. Step = " + mstr(self.DistrStep[i]) + "\n"
                data += "REFL. Size = " + mstr(self.DistrSize[i]) + " [points]\n"
        
        self.outText.set_text(data)
        
    def prepearHead(self):
        data="[INPUT]\n"
        data += "Crystal type = " + self.CrystType + "\n"
        data += "Crystal 2d = " + mstr(self.crystal2d[0]) + " [A]\n"
        data += "Crystal W x H = " + mstr(self.crystalW) + " X " + mstr(self.crystalH) + " [mm]\n"
        data += "Source size W x H= " + mstr(self.SrcSizeW) +" X "+ mstr(self.SrcSizeH) + " [mm]\n"
        data += "Detector size W x H= " + mstr(self.FilmSizeW) + " X " + mstr(self.FilmSizeH) + " [mm]\n"
        data += "Central wavelength = " + mstr(self.centralWave) + " [A]\n"
        data += "Source distance = " + mstr(self.SrcDist) + " [mm]\n"
       
        
        for i in self.DistrSize:
            if (self.DistrSize[i] != 0):
                data += "\n[REFL. FUNCTION IN " + mstr(i + 1) + " ORDER]\n"
                data += "Crystal 2d = " + mstr(self.crystal2d[i]) + " [A]\n"
                data += "REFL. File Name = " + self.DistrFileName[i] + "\n"
                data += "REFL. HW = " + mstr(self.DistrHW[i]) + " [deg]\n"
                data += "REFL. Step = " + mstr(self.DistrStep[i]) + "\n"
                data += "REFL. Size = " + mstr(self.DistrSize[i]) + " [points]\n"
        
        data += "\n\n[OUTPUT]\n"
        data += "Scattering Angle = " + mstr(self.BraggA) + " [deg]\n"
        data += "FRO angle = " + mstr(self.ToFilmDirection) + " [deg]\n"
        data += "Detector angle = " + mstr(self.FilmAngle) + " [deg]\n"
        data += "Detector to crystal = " + mstr(self.DstDist) + " [mm]\n"
        data += "Detector to center = " + mstr(self.FilmDistFromCenter) + " [mm]\n"
        
        data += "\n\n[RESULTS]\n"
        
        return data
    
    def progressLogger(self,ew,es):
        self.progressInfoOut.set_fraction(0.0)
        
        while (not self.exitMainCompThread):
            ptr = c_char_p.in_dll(self.rayTraceLib,"plinkedLibraryOutput")  
            if ptr.value != None:
                progress = ptr.value.split(":")[0]
                progress = progress.replace("[","") 
                progress = progress.replace("]","")
                try:
                    current = int(progress.split("/")[0])
                    total = int(progress.split("/")[1])
                    self.progressInfoOut.set_fraction(float(current) / float(total))
                    self.progressInfoOut.set_text(ptr.value)
                except ValueError:
                    current = 0
                    self.progressInfoOut.set_text(ptr.value)
                         
            Gtk.main_iteration()
            
            time.sleep(0.0001)
            
        return
    
    def execThread(self, params, ew,es):
        loc = locale.getlocale(locale.LC_ALL)
        if sys.platform.startswith('linux'):
            locale.setlocale(locale.LC_ALL, 'en_US.UTF-8')
        else:
            locale.setlocale(locale.LC_ALL,'English_United States') 
               
        self.rayTraceLib.RayTracing(1, params)
                
        locale.setlocale(locale.LC_ALL,loc) 

        self.exitMainCompThread = True
        return

    def execRayTrace(self,parFiles, mainOrder = 1):
        
        self.resPlot.ClearAll()
        
        self.isCompute = 0
        
        Result = dict()
        
        for par in parFiles:
            
            if sys.platform.startswith('linux'):
                print ('OS Linux')
                pFileName=par.split("/")[1].split(".")[0]
                
                os.system("rm -rf results/"+pFileName)
                os.system("rm results/"+pFileName+".dump")
                os.system("rm results/"+pFileName+".log")
                
                                             
                self.exitMainCompThread = False  
                t1 = threading.Thread(target = self.execThread, args=(par, 0, 0))               
                t1.start()
                
                self.progressLogger(0, 0)  
                t1.join()                           

                Result[int(pFileName.split('_')[1])] = self.resPlot.plotReflResults("results/"+pFileName+".log")
                
                self.resPlot.plotWaveLengthUp(self.WaveLengths, self.WaveLimits)
                self.resPlot.plotMirror("results/"+pFileName+".dump",self.crystalW,self.crystalH)
                                
                self.isCompute = len(parFiles)
            else:
                print ('OS Windows')
                pFileName=par.split("/")[1].split(".")[0]
                
                os.system("del /Q results\\"+pFileName)
                os.system("del /Q results\\"+pFileName+".dump")
                os.system("del /Q results\\"+pFileName+".log")
                
                par.replace('/','\\')
                
                self.exitMainCompThread = False  
                t1 = threading.Thread(target = self.execThread, args=(par, 0, 0))               
                t1.start()
                
                self.progressLogger(0, 0)  
                t1.join()

                par.replace('\\','/')
                
                try:
                    Result[int(pFileName.split('_')[1])] = self.resPlot.plotReflResults("results\\"+pFileName+".log")
                    self.resPlot.plotWaveLengthUp(self.WaveLengths, self.WaveLimits)
                    self.resPlot.plotMirror("results\\"+pFileName+".dump",self.crystalW,self.crystalH)
                except (ValueError):
                    return False
                    print("Catch ValueError exeption!!!!. Processing incomplete.")
                except KeyError:
                    print("Catch KeyError exeption!!!!. Processing incomplete.")
                except MemoryError:
                	print("Catch MemoryError exeption!!!!. Processing incomplete.")
                
                #list = glob.glob("results\\"+pFileName+"\\"+"*.dmp")
                #self.resPlot.plotFilm(list[0],self.zeroWave,self.WaveLengths)
                
                self.isCompute = len(parFiles)
                
        list = []
        for par in parFiles:
            if sys.platform.startswith('linux'):
                pFileName=par.split("/")[1].split(".")[0]    
                list += [glob.glob("results/"+pFileName+"/"+"*.dmp")]  
            else:
                pFileName=par.split("/")[1].split(".")[0]    
                list += [glob.glob("results\\"+pFileName+"\\"+"*.dmp")]
            
        print( list )

        self.resPlot.plotFilm(list,self.zeroWave,self.WaveLengths, self.crystal2d, self.Orders, mainOrder)
            
        return Result
                    
    def dispError(self, text):
        self.outText.set_text(text)    
        
    def setDistrFile(self, fileName, distrOrder):
        f = open(fileName, 'r')
        lines = f.readlines()
        x = []
        y = []
        for line in lines:
            xt, yt = (float(i) for i in line.split())
            x += [xt]
            y += [yt]
        
        self.DistrFileName[distrOrder] = fileName
        self.DistrHW[distrOrder] = ( max(x) + math.fabs(min(x)) ) / 2.0
                
        self.DistrStep[distrOrder] = x[1] - x[0]
        self.DistrSize[distrOrder] = len(x)
        self.dispData()
                
    def updateConfigNCW(self):

        centralWave = self.centralWave * self.mainOrder / (
                        self.crystal2d[0] / self.crystal2d[self.mainOrder - 1])

        sTheta = centralWave / self.crystal2d[0]
        if (sTheta > 1.0):
            self.dispError("Bad central wave for selected order. Use wavelength less than " + mstr(self.crystal2d[0]) + " [A]")
            return
        
        self.BraggA = math.asin(sTheta) * 180.0 / math.pi
        
        phi = (90.0 - self.BraggA) * math.pi / 180.0
        
        self.DstDist = self.SrcDist * self.crystalR / (2.0 * self.SrcDist * math.cos(phi) - self.crystalR)
        self.FilmDistFromCenter = math.sqrt(self.DstDist ** 2 + self.crystalR ** 2 / 4.0 
                                            - self.DstDist * self.crystalR * math.cos(phi))
        
        self.ToFilmDirection = math.pi - math.asin(self.DstDist * math.sin(phi) / self.FilmDistFromCenter)
        self.ToFilmDirection *= 180.0 / math.pi
        
        L = math.sqrt(self.SrcDist ** 2 + self.DstDist ** 2 - 2 * self.SrcDist * self.DstDist * math.cos(2 * phi))
        sGamma = self.SrcDist * math.sin(2 * phi) / L
        Gamma = math.asin(1.0 if math.fabs(sGamma) > 1 else sGamma)
        Gamma2 = math.asin(self.crystalR * math.sin(phi) / (2 * self.FilmDistFromCenter))
        self.FilmAngle = (Gamma - Gamma2) * 180.0 / math.pi;
        
        theta = self.BraggA * math.pi / 180.0
        H=self.crystalR - math.sqrt(self.crystalR**2 - self.crystalW**2/4.0)
        dh=H*math.tan(math.pi/2.0 - theta)
        l=self.crystalW / 2.0 + dh
        L=self.SrcDist - math.sqrt(dh**2 + H**2)
        
        d = L**2 + l**2 - 2*L*l*math.sin(math.pi - theta)
        d = d**0.5
        
        self.SrcCone = 2 * math.asin(l / d * math.sin(math.pi - theta))
        self.SrcCone *= 180.0 / math.pi *1.05
        
        self.dispData()
        
    def updateConfig(self):
        phi = (90.0 - self.BraggA) * math.pi / 180.0
        self.DstDist = self.SrcDist * self.crystalR / (2.0 * self.SrcDist * math.cos(phi) - self.crystalR)
        self.FilmDistFromCenter = math.sqrt(self.DstDist ** 2 + self.crystalR ** 2 / 4.0 
                                            - self.DstDist * self.crystalR * math.cos(phi))
        
        self.ToFilmDirection = math.pi - math.asin(self.DstDist * math.sin(phi) / self.FilmDistFromCenter)
        self.ToFilmDirection *= 180.0 / math.pi
        
        L = math.sqrt(self.SrcDist ** 2 + self.DstDist ** 2 - 2 * self.SrcDist * self.DstDist * math.cos(2 * phi))
        sGamma = self.SrcDist * math.sin(2 * phi) / L
        Gamma = math.asin(1.0 if math.fabs(sGamma) > 1 else sGamma)
        Gamma2 = math.asin(self.crystalR * math.sin(phi) / (2 * self.FilmDistFromCenter))
        self.FilmAngle = (Gamma - Gamma2) * 180.0 / math.pi;
        
        theta = self.BraggA * math.pi / 180.0
        H=self.crystalR - math.sqrt(self.crystalR**2 - self.crystalW**2/4.0)
        dh=H*math.tan(math.pi/2.0 - theta)
        l=self.crystalW / 2.0 + dh
        L=self.SrcDist - math.sqrt(dh**2 + H**2)
        
        d = L**2 + l**2 - 2*L*l*math.sin(math.pi - theta)
        d = d**0.5
        
        self.SrcCone = 2 * math.asin(l / d * math.sin(math.pi - theta))
        self.SrcCone *= 180.0 / math.pi *1.05
        
        self.dispData()
    
    def findWaveLength(self, elems):
        f = open("sys/trans-db.txt", "r")
        Name = []
        Wls = []
        self.WaveLengths = []
        linesData=f.readlines()
        for elem in elems:
            for l in linesData:
                lsp = l.split()
                if (len(lsp) >= 4 and elem in l):
                    El = lsp[0]
                    Line = lsp[1] 
                    Type = lsp[2]
                    WaveLength = lsp[3] 
                    
                    Name += [El + " " + Line + " " + Type]                
                    Wls += [round(float(WaveLength.replace(',', '.')),6)]
                    #self.WaveLengths += [float(WaveLength.replace(',', '.'))]
                    
        return Name, Wls 
    
    def setWaveLength(self, WaveLength, WaveIntens, WaveWidth):
        self.WaveLengths = list(WaveLength)
        self.WaveIntensity = list(WaveIntens)
        self.dWaveLength = list(WaveWidth)
        
        if (self.WaveAutoscale == True):
            self.resPlot.plotWaveLength(self.WaveLengths)
        else:
            self.resPlot.plotWaveLength(self.WaveLengths, self.WaveLimits)
    
    def computFSSR1(self):
        centralWave = self.centralWave * self.mainOrder / (
                        self.crystal2d[0] / self.crystal2d[self.mainOrder - 1])

        sTheta = centralWave / self.crystal2d[self.mainOrder - 1]
        if (sTheta > 1.0):
            self.dispError("Bad central wave for selected order. Use wavelength less than " + mstr(self.crystal2d[self.mainOrder - 1]) + " [A]")
            return
        
        self.BraggA = math.asin(sTheta) * 180.0 / math.pi
        
        phi = (90.0 - self.BraggA) * math.pi / 180.0
        self.SrcDist = self.crystalR * math.cos(phi) / math.cos(2 * phi)
        self.DstDist = self.crystalR * math.cos(phi)
        
        self.FilmDistFromCenter = math.sqrt(self.DstDist ** 2 + self.crystalR ** 2 / 4.0 
                                            - self.DstDist * self.crystalR * math.cos(phi))
        
        self.ToFilmDirection = math.pi - math.asin(self.DstDist * math.sin(phi) / self.FilmDistFromCenter)
        self.ToFilmDirection *= 180.0 / math.pi
        
        L = math.sqrt(self.SrcDist ** 2 + self.DstDist ** 2 - 2 * self.SrcDist * self.DstDist * math.cos(2 * phi))
        sGamma = self.SrcDist * math.sin(2 * phi) / L
        Gamma = math.asin(1.0 if math.fabs(sGamma) > 1 else sGamma)
        Gamma2 = math.asin(self.crystalR * math.sin(phi) / (2 * self.FilmDistFromCenter))
        self.FilmAngle = (Gamma - Gamma2) * 180.0 / math.pi;
        
        theta = self.BraggA * math.pi / 180.0
        
        H=self.crystalR - math.sqrt(self.crystalR**2 - self.crystalW**2/4.0)
        dh=H*math.tan(math.pi/2.0 - theta)
        l=self.crystalW / 2.0 + dh
        L=self.SrcDist - math.sqrt(dh**2 + H**2)
        
        d = L**2 + l**2 - 2*L*l*math.sin(math.pi - theta)
        d = d**0.5
        
        self.SrcCone = 2 * math.asin(l / d * math.sin(math.pi - theta))
        self.SrcCone *= 180.0 / math.pi *1.05
        
        self.dispData()
