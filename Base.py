# coding: utf-8
'''
Created on 20.03.2015

@author: cheshire
'''
import sys

import confCompute
from resultsOutput import *
from confCompute import *
import numpy as np
import matplotlib.pyplot as plt

if sys.platform.startswith('linux'):
    from gi.repository import Gtk
    from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg as FigureCanvas
else:
    import gtk as Gtk
    from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas


def isfloat(value):
    try:
        float(value)
        return True
    except (ValueError):
        return False


def mstr(val):
    try:
        basestring
    except NameError:
        basestring = str

    if (isinstance(val, basestring)):
        return val
    if (type(val) == int):
        return "%d" % val
    return "%0.7g" % val


class spGeneral:
    saveFile = ''
    isSave = 0
    isCentralLineSelected = False

    def __init__(self):
        print ('Init class spGeneral')

    def updWaveLength(self):
        toAdd = []
        toAddInt = []
        toAddWw = []
        toAddOrder = []
        for wl in self.wlSelStore:
            if (not wl[1] == 0):
                toAdd += [round(wl[1], 6)]
                toAddInt += [round(wl[11], 2)]
                toAddWw += [round(wl[10], 4)]
                toAddOrder += [wl[8]]

        self.sys.setWaveLength(toAdd, toAddInt, toAddWw, toAddOrder)

    def on_treeview2_key_press_event(self, obj, event):
        if (event.keyval == 65535):
            selected = self.TreeViev.get_selection()
            model, titer = selected.get_selected()
            if titer is not None:
                self.wlSelStore.remove(titer)

            self.updWaveLength()

    def on_button1_clicked(self, obj):
        self.sys.centralWave = float(self.entry_cwl.get_text())
        self.sys.crystalR = float(self.entry_R.get_text())

        # self.sys.crystal2d[0]=float(self.entry_2d.get_text())

        self.sys.crystalW = float(self.entry_cW.get_text())
        self.sys.crystalH = float(self.entry_cH.get_text())
        self.sys.RayCount = int(self.entry_numRays.get_text())
        self.sys.RayByIter = int(self.entry_iterRays.get_text())

        self.sys.SrcSizeW = float(self.entry_srcSize.get_text())
        self.sys.SrcSizeH = float(self.entry_srcSizeH.get_text())

        self.sys.FilmSizeW = float(self.entry_filmSizeW.get_text())
        self.sys.FilmSizeH = float(self.entry_filmSizeH.get_text())

        index = self.crystalType.get_active()
        model = self.crystalType.get_model()
        self.sys.CrystType = model[index][0]

        self.sys.computFSSR1()
        self.entry_srcDist.set_text(self.sys.get_srcDist())
        self.entry_Bragg.set_text(self.sys.get_bragg())

        if (self.isCentralLineSelected):
            self.setWaveLimits()

        self.updWaveLength()

    def on_cellrenderertoggle1_toggled(self, obj, obj1):
        for i, c in enumerate(self.wlSelStore):
            if not i == obj1:
                self.wlSelStore[i][9] = 0

        self.wlSelStore[obj1][9] = 1
        self.entry_zeroWave.set_text(self.wlSelStore[obj1][0] + "; " + mstr(self.wlSelStore[obj1][1]))
        self.entry_cwl.set_text(mstr(self.wlSelStore[obj1][1]))

    def on_entry19_changed(self, obj):
        try:
            self.sys.resPlot.pixel_size = float(self.entry_pixelSize.get_text())
            self.on_entry14_changed(self.entry_zeroOrder)
        except ValueError:
            pass

    def on_entry12_changed(self, obj):
        ElementsName = self.entry_Wl.get_text().split(';')
        name, wl, dwl, Intensity = self.sys.findWaveLength(ElementsName)

        self.wlStore.clear()

        for i, el in enumerate(name):
            self.wlStore.append()
            self.wlStore[len(self.wlStore) - 1][0] = name[i]
            self.wlStore[len(self.wlStore) - 1][1] = wl[i]
            self.wlStore[len(self.wlStore) - 1][2] = dwl[i]
            self.wlStore[len(self.wlStore) - 1][3] = Intensity[i]

    def on_button2_clicked(self, obj):
        self.sys.centralWave = float(self.entry_cwl.get_text())
        self.sys.crystalR = float(self.entry_R.get_text())
        self.sys.resPlot.pixel_size = float(self.entry_pixelSize.get_text())

        self.sys.crystalW = float(self.entry_cW.get_text())
        self.sys.crystalH = float(self.entry_cH.get_text())
        self.sys.RayCount = int(self.entry_numRays.get_text())
        self.sys.RayByIter = int(self.entry_iterRays.get_text())
        self.sys.BraggA = float(self.entry_Bragg.get_text())
        self.sys.SrcDist = float(self.entry_srcDist.get_text())

        self.sys.SrcSizeW = float(self.entry_srcSize.get_text())
        self.sys.SrcSizeH = float(self.entry_srcSizeH.get_text())

        self.sys.FilmSizeW = float(self.entry_filmSizeW.get_text())
        self.sys.FilmSizeH = float(self.entry_filmSizeH.get_text())

        index = self.crystalType.get_active()
        model = self.crystalType.get_model()
        self.sys.CrystType = model[index][0]

        self.sys.updateConfig()
        self.entry_srcDist.set_text(self.sys.get_srcDist())
        self.entry_Bragg.set_text(self.sys.get_bragg())

        if (self.isCentralLineSelected):
            self.setWaveLimits()

        self.updWaveLength()

    def setWaveLimits(self):
        [w1, w2, p1, p2] = self.sys.calcLimWaveLength(int(self.entry_zeroOrder.get_text()))
        self.sys.WaveLimits = [w1, w2]
        self.MaxWl[0].set_text(mstr(w1))
        self.MinWl[0].set_text(mstr(w2))
        self.MaxWl[1].set_text(mstr(p1))
        self.MinWl[1].set_text(mstr(p2))

    def on_entry2_changed(self, obj):
        self.sys.crystalR = float(self.entry_R.get_text())
        self.sys.SrcDist = float(self.entry_srcDist.get_text())
        self.sys.updateConfig()
        self.entry_srcDist.set_text(self.sys.get_srcDist())
        self.entry_Bragg.set_text(self.sys.get_bragg())
        self.updWaveLength()

    def on_entry3_changed(self, obj):
        self.sys.centralWave = float(self.entry_cwl.get_text())
        self.sys.SrcDist = float(self.entry_srcDist.get_text())
        self.sys.updateConfigNCW()
        self.entry_srcDist.set_text(self.sys.get_srcDist())
        self.entry_Bragg.set_text(self.sys.get_bragg())

        self.isCentralLineSelected = True
        self.setWaveLimits()

        self.updWaveLength()

    def on_entry13_changed(self, obj):
        self.sys.BraggA = float(self.entry_Bragg.get_text())
        self.sys.SrcDist = float(self.entry_srcDist.get_text())
        self.sys.updateConfig()
        self.entry_srcDist.set_text(self.sys.get_srcDist())

        if (self.isCentralLineSelected):
            self.setWaveLimits()

        self.updWaveLength()

    def on_entry11_changed(self, obj):
        self.sys.SrcDist = float(self.entry_srcDist.get_text())
        self.sys.updateConfig()
        self.entry_Bragg.set_text(self.sys.get_bragg())

        if (self.isCentralLineSelected):
            self.setWaveLimits()

        self.updWaveLength()

    def on_window1_destroy(self, obj):
        print("Quit!")
        sys.exit(0)

    def on_window1_delete_event(self, obj, data):
        Gtk.main_quit()
        self.sys.free_lib()
        self.fileChoose.destroy()
        self.winProgress.destroy()
        self.chButton2.destroy()
        self.win.destroy()
        self.ImageSaveWindow.destroy()

    def on_filechooserbutton3_file_set(self, obj):
        self.saveFile = obj.get_filenames()[0]

    def on_filechooserbutton2_file_set(self, obj):
        FileName = self.chButton2.get_filenames()

        fPtr = open(FileName[0], "r+")
        fLines = fPtr.readlines()

        isRead = 0

        self.sys.crystClear()

        for sLine in fLines:
            if (isRead == 1):
                data = sLine.split()
                distrOrder = int(data[0]) - 1
                self.sys.crystal2d[distrOrder] = float(data[1])
                self.sys.setDistrFile(data[2], distrOrder)

            if (not sLine.find("***") == -1):
                self.sys.crystal2d = {}
                isRead = 1

        self.entry_2d.set_text("".join((mstr(self.sys.crystal2d[a]) + " " for a in self.sys.crystal2d)));

    def on_cellrenderertext3_edited(self, obj1, obj2, obj3):
        self.wlSelStore[obj2][0] = obj3
        self.updWaveLength()

    def on_cellrenderertext13_edited(self, obj1, obj2, obj3):
        self.wlSelStore[obj2][10] = float(obj3.replace(',', '.'))
        self.updWaveLength()

    def on_cellrenderertext14_edited(self, obj1, obj2, obj3):
        self.wlSelStore[obj2][11] = float(obj3.replace(',', '.'))
        self.updWaveLength()

    def on_cellrenderertext4_edited(self, obj1, obj2, obj3):
        self.wlSelStore[obj2][1] = float(obj3.replace(',', '.'))
        self.wlSelStore[obj2][10] = self.wlSelStore[obj2][1]
        self.wlSelStore[obj2][11] = 1.0
        self.updWaveLength()

        if (len(self.wlSelStore) == int(obj2) + 1):
            self.wlSelStore.append([mstr(int(obj2) + 1), 0, 0, 0, 0, 0, 0, 0, self.sys.mainOrder, 0, 0, 1.0])

    def on_button11_clicked(self, obj1):
        additional_order_line = []
        Orders = [int(a) for a in self.entry_Orders.get_text().split()]
        MainOrder = int(self.entry_zeroOrder.get_text())
        current_lines_names = []
        CurrentLines = []
        for line in self.wlSelStore:
            if "_order_" not in line[0]:
                CurrentLines.append(line)
            current_lines_names.append(line[0])

        for current_line in CurrentLines:
            for order in Orders:
                new_line = list(current_line)
                current_order = new_line[8]
                if current_order != order:
                    new_line[1] = current_line[1] * order / current_order * (
                        self.sys.crystal2d[order - 1] / self.sys.crystal2d[current_order - 1])
                    new_line[10] = new_line[1]
                    new_line[0] = current_line[0] + "_order_" + str(order)

                    new_line = [new_line[0], new_line[1], 0, 0, 0, 0, 0, 0, order, 0, new_line[10],
                                current_line[11] * float(current_order) / float(order)]
                    if not new_line[0] in current_lines_names:
                        additional_order_line.append(new_line)

        for new_line in additional_order_line:
            self.wlSelStore.append(new_line)

        self.updWaveLength()

    def on_button15_clicked(self, obj):

        for line in self.wlSelStore:
            if "_order_" in line[0]:
                self.wlSelStore.remove(line.iter)

    def on_button16_clicked(self, obj):
        self.winProgress.hide()
        self.sys.TerminateCalculation()

    def on_treeview1_row_activated(self, obj1, obj2, obj3):

        if (self.isCentralLineSelected):
            self.setWaveLimits()

        selected = self.TreeVievPrep.get_selection()
        model, titer = selected.get_selected()
        if titer is not None:
            toAppend = [model.get_value(titer, 0), model.get_value(titer, 1), 0, 0, 0, 0, 0, 0, self.sys.mainOrder, 0,
                        model.get_value(titer, 2), model.get_value(titer, 3)]
            self.wlSelStore.append(toAppend)

        self.updWaveLength()

    def on_button3_clicked(self, obj):
        selected = self.TreeVievPrep.get_selection()
        model, titer = selected.get_selected()
        if titer is not None:
            toAppend = [model.get_value(titer, 0), model.get_value(titer, 1), 0, 0, 0, 0, 0, 0, self.sys.mainOrder, 0,
                        model.get_value(titer, 2), model.get_value(titer, 3)]
            self.wlSelStore.append(toAppend)

        if (self.isCentralLineSelected):
            self.setWaveLimits()

        self.updWaveLength()

    def on_button10_clicked(self, obj):
        self.wlSelStore.clear()
        self.updWaveLength()

    def on_checkbutton1_toggled(self, obj):
        self.sys.WaveAutoscale = obj.get_active()
        if (self.isCentralLineSelected):
            self.setWaveLimits()
        self.updWaveLength()

    def on_button9_clicked(self, obj):
        if (self.isCentralLineSelected):
            self.setWaveLimits()

        [w1, w2, p1, p2] = self.sys.calcLimWaveLength(int(self.entry_zeroOrder.get_text()))
        wMin = w2
        wMax = w1 - w1 * 0.001

        cw = (wMin + wMax) * 0.5
        setOfWave = [wMin]
        setOfWave += genSetOfWave(wMin, cw, 0, 1, 8)
        setOfWave += [cw]
        setOfWave += [-i for i in genSetOfWave(-wMax, -cw, 0, 1, 8)]
        setOfWave += [wMax]

        for i in setOfWave:
            toAppend = ['WL_' + mstr(i), i, 0, 0, 0, 0, 0, 0, 0, 0, i, 1.0]
            self.wlSelStore.append(toAppend)

        self.updWaveLength()

    def on_button4_clicked(self, obj):
        selected = self.TreeViev.get_selection()
        model, titer = selected.get_selected()
        if titer is not None:
            self.wlSelStore.remove(titer)

        self.updWaveLength()

    def on_entry7_changed(self, obj):
        zeroWaveTex = self.entry_zeroWave.get_text()
        zeroWaveTex = zeroWaveTex.split(';')

        if (isfloat(zeroWaveTex[0])):
            self.sys.zeroWave = float(zeroWaveTex[0])
        elif (len(zeroWaveTex) == 0):
            self.sys.zeroWave = self.sys.centralWave
        elif (len(zeroWaveTex) == 1 or zeroWaveTex[1] == ""):
            self.sys.zeroWave = self.sys.centralWave
            for i in self.wlSelStore:
                if zeroWaveTex[0] in i[0]:
                    self.sys.zeroWave = i[1]
        else:
            self.sys.zeroWave = float(zeroWaveTex[1])

        self.label_zeroWave.set_text(" " + u'\u03BB' + "  = " + mstr(self.sys.zeroWave))

        if self.sys.isCompute != 0:
            order = self.entry_zeroOrder.get_text()
            order = 1 if order == "" else int(order)

            Orders = [int(a) for a in self.entry_zeroOrder.get_text().split()]

            list = []
            for o in Orders:
                pFileName = "Order_" + mstr(o)
                if sys.platform.startswith('linux'):
                    list += [glob.glob("results/" + pFileName + "/" + "*.dmp")]
                else:
                    list += [glob.glob("results\\" + pFileName + "\\" + "*.dmp")]

            if (not len(list) == 0):
                zero_wave_index = self.sys.WaveLengths.index(self.sys.zeroWave)

                dzero_wave =self.sys.dWaveLength[zero_wave_index]
                self.sys.resPlot.plotFilm(list, self.sys.zeroWave, self.sys.crystal2d, Orders,
                                          order, dzero_wave)

    def on_treeview2_button_press_event(self, obj, buttn):
        if (buttn.button == 3):
            selected = self.TreeViev.get_selection()
            model, titer = selected.get_selected()
            if titer is not None:
                self.entry_zeroWave.set_text(mstr(model.get_value(titer, 0)) + "; " + mstr(model.get_value(titer, 1)))
                self.isCentralLineSelected = True

    def on_button6_clicked(self, obj):
        self.fileChoose.show_all()

    def on_button12_clicked(self, obj):
        self.ImageSaveWindow.show_all()

    def on_button14_clicked(self, obj):
        self.ImageSaveWindow.hide()

    def on_button13_clicked(self, obj):
        self.ImageSaveWindow.hide()

        save_file_patter = self.ImageSaveWindow.get_filename()

        film_file_name = save_file_patter + "_film.png"
        self.sys.resPlot.filmFig.savefig(film_file_name, dpi=600)

        mirror_file_name = save_file_patter + "_mirror.png"
        self.sys.resPlot.mirrorFig.savefig(mirror_file_name, dpi=600)

        spectrum_file_name = save_file_patter + "_spectr.eps"
        self.sys.resPlot.xResFig.savefig(spectrum_file_name)

        spectrum_file_name_txt = save_file_patter + "_spectr.txt"

        file_object = open(spectrum_file_name_txt, "w+")
        for line in self.sys.resPlot.xResAxix.lines:
            xd = line.get_xdata()
            yd = line.get_ydata()
            file_object.write("New line\n")
            file_object.write(xd, yd)

    def on_button7_clicked(self, obj):
        self.saveFile = self.fileChoose.get_filename()
        self.button_fc.set_label(os.path.basename(self.saveFile))
        self.fileChoose.hide()

        self.isSave = 1
        self.dumpDataToLog()

    def on_button8_clicked(self, obj):
        self.fileChoose.hide()

    def setDetLimits(self):
        for i in [0, 1]:
            self.entryXlim[i].set_text(mstr(self.detXlimits[i]))
            self.entryYlim[i].set_text(mstr(self.detYlimits[i]))

        return

    def getDetLimits(self):
        self.detYlimits = list(self.resOut.filmAxix.get_ylim())
        self.detXlimits = list(self.resOut.filmAxix.get_xlim())

        return

    def on_entry15_changed(self, obj):  # Xmin
        try:
            val = float(obj.get_text())
            self.detXlimits[0] = val if (val < self.detXlimits[1]) else self.detXlimits[0]
            self.resOut.changeDetectorAxixLimits(self.detXlimits, self.detYlimits)
            # self.setDetLimits()
        except (ValueError, TypeError):
            return

    def on_entry22_changed(self, obj):  # Xmax
        try:
            val = float(obj.get_text())
            self.detXlimits[1] = val if (val > self.detXlimits[0]) else self.detXlimits[1]
            self.resOut.changeDetectorAxixLimits(self.detXlimits, self.detYlimits)
            # self.setDetLimits()
        except (ValueError, TypeError):
            return

    def on_entry28_changed(self, obj):  # Ymin
        try:
            val = float(obj.get_text())
            self.detYlimits[0] = val if (val < self.detYlimits[1]) else self.detYlimits[0]
            self.resOut.changeDetectorAxixLimits(self.detXlimits, self.detYlimits)
            # self.setDetLimits()
        except (ValueError, TypeError):
            return

    def on_entry24_changed(self, obj):  # Xmax
        try:
            val = float(obj.get_text())
            self.detYlimits[1] = val if (val > self.detYlimits[0]) else self.detYlimits[1]
            self.resOut.changeDetectorAxixLimits(self.detXlimits, self.detYlimits)
            # self.setDetLimits()
        except (ValueError, TypeError):
            return

    def on_button5_clicked(self, obj):
        self.sys.centralWave = float(self.entry_cwl.get_text())
        self.sys.crystalR = float(self.entry_R.get_text())

        self.sys.crystalW = float(self.entry_cW.get_text())
        self.sys.crystalH = float(self.entry_cH.get_text())
        self.sys.RayCount = int(self.entry_numRays.get_text())
        self.sys.RayByIter = int(self.entry_iterRays.get_text())
        self.sys.BraggA = float(self.entry_Bragg.get_text())
        self.sys.SrcDist = float(self.entry_srcDist.get_text())

        self.sys.SrcSizeW = float(self.entry_srcSize.get_text())
        self.sys.SrcSizeH = float(self.entry_srcSizeH.get_text())

        self.sys.FilmSizeW = float(self.entry_filmSizeW.get_text())
        self.sys.FilmSizeH = float(self.entry_filmSizeH.get_text())

        self.sys.resPlot.pixel_size = float(self.entry_pixelSize.get_text())

        index = self.crystalType.get_active()
        model = self.crystalType.get_model()
        self.sys.CrystType = model[index][0]

        self.sys.updateConfig()
        self.updWaveLength()

        self.entry_srcDist.set_text(self.sys.get_srcDist())
        self.entry_Bragg.set_text(self.sys.get_bragg())

        #####

        zeroWaveTex = self.entry_zeroWave.get_text()
        zeroWaveTex = zeroWaveTex.split(';')

        if (isfloat(zeroWaveTex[0])):
            self.sys.zeroWave = float(zeroWaveTex[0])
        elif (len(zeroWaveTex) == 0):
            self.sys.zeroWave = self.sys.centralWave
        elif (len(zeroWaveTex) == 1 or zeroWaveTex[1] == ""):
            self.sys.zeroWave = self.sys.centralWave
            for i in self.wlSelStore:
                if zeroWaveTex[0] in i[0]:
                    self.sys.zeroWave = i[1]
        else:
            self.sys.zeroWave = float(zeroWaveTex[1])

        self.label_zeroWave.set_text(" " + u'\u03BB' + " = " + mstr(self.sys.zeroWave))

        wlCount = len(self.sys.WaveLengths) - 2

        self.winProgress.show_all()
        ##########################
        self.sys.Orders = [int(a) for a in self.entry_zeroOrder.get_text().split()]
        deforder = int(self.entry_zeroOrder.get_text()) if int(self.entry_zeroOrder.get_text()) in self.sys.Orders else self.sys.Orders[0]

        self.on_entry14_changed(self.entry_zeroOrder)

        parFiles = self.sys.prepareComputation()
        self.Res = self.sys.execRayTrace(parFiles, deforder)

        ########################## 
        self.winProgress.hide()

        self.FilmResult = {}
        self.sys.zeroWave = round(self.sys.zeroWave, 6)

        for order in self.sys.Orders:
            pFileName = "Order_" + mstr(order)
            if sys.platform.startswith('linux'):
                list = glob.glob("results/" + pFileName + "/" + "*.dmp")
            else:
                list = glob.glob("results\\" + pFileName + "\\" + "*.dmp")

            self.FilmResult[order] = self.sys.resPlot.analizeFilm(list[0], self.sys.WaveLengths, self.sys.dWaveLength,
                                                                  self.sys.zeroWave)
            if order == deforder:
                if len(self.sys.resPlot.dispCurve) == 0:
                    self.dispCurve = "No image on detector"
                else:
                    try:
                        self.dispCurve = self.sys.resPlot.dispCurve[int(self.sys.zeroWave * 1e+6)]
                    except (KeyError):
                        self.dispCurve = "Key Error"
                        print ("Key Error in dispCurve dict")

        self.addDataToTable(deforder)

        self.getDetLimits()
        self.setDetLimits()

        self.dumpDataToLog()

    def on_entry14_changed(self, obj):
        try:
            self.sys.mainOrder = int(self.entry_zeroOrder.get_text())
        except ValueError:
            pass
        self.updWaveLength()
        if (isfloat(obj.get_text()) and self.sys.isCompute):
            if (not self.sys.isCompute == 0):
                order = self.entry_zeroOrder.get_text()

                Orders = [int(a) for a in self.entry_zeroOrder.get_text().split()]

                if (self.isCentralLineSelected):
                    self.setWaveLimits()
                    self.updWaveLength()

                list = []
                for o in Orders:
                    pFileName = "Order_" + mstr(o)
                    if sys.platform.startswith('linux'):
                        list += [glob.glob("results/" + pFileName + "/" + "*.dmp")]
                    else:
                        list += [glob.glob("results\\" + pFileName + "\\" + "*.dmp")]

                if (not len(list) == 0):
                    zero_wave_index = self.sys.WaveLengths.index(self.sys.zeroWave)
                    dzero_wave = self.sys.dWaveLength[zero_wave_index]
                    self.sys.resPlot.plotFilm(list, self.sys.zeroWave, self.sys.crystal2d, Orders,
                                              order, dzero_wave)

                self.addDataToTable(int(order))

    def dumpDataToLog(self):
        data = self.sys.prepearHead()

        data += "Reference line = " + self.entry_zeroWave.get_text() + "\n";
        data += "Dispersion curve for " + self.entry_zeroOrder.get_text() + " order = [" + self.dispCurve + "]\n"

        self.setWaveLimits()

        data += "Reflection limits for " + self.entry_zeroOrder.get_text() + " order\n"
        data += "Min wavelength = " + self.MinWl[0].get_text() + "\tPosition = " + self.MinWl[1].get_text() + "\n"
        data += "Min wavelength = " + self.MaxWl[0].get_text() + "\tPosition = " + self.MaxWl[1].get_text() + "\n"

        data += "***************************************************************\n"
        data += "Name\tWavelength\tCaptured\tReflected\tReflectivity\tCoords\tMagnification\tFWHM\tOrder\t"
        data += "isRefLine\tLine Width\tLine intencity\n"

        for order in self.sys.Orders:
            self.addDataToTable(order)
            data += "Order:\t" + mstr(order) + "\n"
            for i, el in enumerate(self.wlSelStore):
                for d in el:
                    data += mstr(d) + "\t"
                data += "\n"

        order = int(self.entry_zeroOrder.get_text()) if int(self.entry_zeroOrder.get_text()) in self.sys.Orders else self.sys.Orders[0]
        self.addDataToTable(order)

        data += "\n\nAll Dispersion curves in " +self.entry_zeroOrder.get_text() + " order:\n"
        for key, value in self.resOut.dispCurve.items():
            data += "For u'\u03BB' = "+mstr(key / 1e+6) + ":\t" + value + "\n"

        if self.isSave == 1:
            if self.saveFile == '':
                self.saveFile = "out.log"

            f = open(self.saveFile, "w")
            try:
                f.write(data)
            except UnicodeEncodeError:
                f.write(data.encode("utf-8"))

            f.close()
            self.isSave = 0

        self.outBuf.set_text(data)

    def getIdxInSelStore(self, wl):
        for i, w in enumerate(self.wlSelStore):
            if (math.fabs(w[1] - wl) <= 1e-5):
                return i

        return -1

    def addDataToTable(self, order):
        if (order in self.Res):
            for i, el in enumerate(self.Res[order]):
                idx = self.getIdxInSelStore(el[0])

                if idx == -1:
                    continue

                self.wlSelStore[idx][2] = int(el[1])
                self.wlSelStore[idx][3] = int(el[2])
                self.wlSelStore[idx][4] = float(el[3])

        if (order in self.FilmResult):
            for wl in self.FilmResult[order]:
                idx = self.getIdxInSelStore(wl[0])

                if idx == -1:
                    continue

                self.wlSelStore[idx][5] = float(wl[1])

                # self.wlSelStore[idx][6]=float(wl[2])/self.sys.SrcSizeW

                self.wlSelStore[idx][6] = float(wl[2]) / self.sys.SrcSizeH
                self.wlSelStore[idx][7] = float(wl[3])

    def initWindow(self):
        self.builder = Gtk.Builder()
        self.builder.add_objects_from_file('sys/spectr.glade', ('liststore1', 'liststore2', 'liststore3', 'window1',
                                                                'filechooserdialog1', 'pwindow',
                                                                'filechooserdialog2', ''))
        self.win = self.builder.get_object('window1')
        self.winProgress = self.builder.get_object('pwindow')

        self.fileChoose = self.builder.get_object('filechooserdialog1')
        self.ImageSaveWindow = self.builder.get_object('filechooserdialog2')

        self.wlStore = self.builder.get_object('liststore1')
        self.wlSelStore = self.builder.get_object('liststore2')
        self.TreeVievPrep = self.builder.get_object('treeview1')
        self.TreeViev = self.builder.get_object('treeview2')

        self.entry_2d = self.builder.get_object('entry1')
        self.entry_R = self.builder.get_object('entry2')
        self.entry_cwl = self.builder.get_object('entry3')

        self.entry_zeroWave = self.builder.get_object('entry7')
        self.entry_zeroOrder = self.builder.get_object('entry14')
        self.label_zeroWave = self.builder.get_object('label3')

        self.button_1d = self.builder.get_object('button1')
        self.button_fc = self.builder.get_object('button6')
        self.fcDialog = self.builder.get_object('dialog-vbox1')

        self.entry_Orders = self.builder.get_object('entry4')

        self.entry_cW = self.builder.get_object('entry5')
        self.entry_cH = self.builder.get_object('entry6')

        self.entry_numRays = self.builder.get_object('entry8')
        self.entry_iterRays = self.builder.get_object('entry9')

        self.entry_Wl = self.builder.get_object('entry12')

        self.entry_srcSize = self.builder.get_object('entry10')
        self.entry_srcDist = self.builder.get_object('entry11')
        self.entry_Bragg = self.builder.get_object('entry13')

        self.entry_srcSizeH = self.builder.get_object('entry16')

        self.entry_filmSizeW = self.builder.get_object('entry17')
        self.entry_filmSizeH = self.builder.get_object('entry18')

        self.crystalType = self.builder.get_object('combobox1')

        self.slitPos = self.builder.get_object('entry19')
        self.slitSizeW = self.builder.get_object('entry20')
        self.slitSizeH = self.builder.get_object('entry21')

        self.entry_pixelSize = self.builder.get_object('entry19')

        self.MinWl = [self.builder.get_object('label27'), self.builder.get_object('label35')]
        self.MaxWl = [self.builder.get_object('label28'), self.builder.get_object('label37')]

        self.entryXlim = [self.builder.get_object('entry15'), self.builder.get_object('entry22')]
        self.entryYlim = [self.builder.get_object('entry28'), self.builder.get_object('entry24')]

        self.outText = self.builder.get_object('textview1')

        # self.waveResText=self.builder.get_object('textview2')

        self.chButton2 = self.builder.get_object('filechooserbutton2')

        self.reflDistr = self.builder.get_object('scrolledwindow1')

        self.sw1 = self.builder.get_object('scrolledwindow3')
        self.sw2 = self.builder.get_object('scrolledwindow6')
        self.sw3 = self.builder.get_object('scrolledwindow7')

        self.waveRes = self.builder.get_object('scrolledwindow5')
        self.xRes = self.builder.get_object('scrolledwindow8')

        self.outBuf = self.outText.get_buffer()

        self.builder.connect_signals(self)

        self.sys = systemConfig(self.outBuf)

        self.sys.progressInfoOut = self.builder.get_object('progressbar1');

        self.reflFig = plt.figure(figsize=(10, 10), dpi=100)
        self.reflAx = self.reflFig.add_subplot(111)
        self.reflAx.set_xlabel("Angle, [deg]", labelpad=-33)
        self.reflAx.set_ylabel("Reflection")

        self.mFig = plt.figure(figsize=(10, 10), dpi=100, facecolor="white")
        self.mAx = self.mFig.add_subplot(111)
        self.mAx.set_xlabel("x, [mm]", labelpad=-33)
        self.mAx.set_ylabel("y, [mm]")
        reflCanvas = FigureCanvas(self.mFig)
        self.sw2.add_with_viewport(reflCanvas)

        self.fFig = plt.figure(figsize=(10, 10), dpi=100, facecolor="white")
        self.fAx = self.fFig.add_subplot(111)
        self.fAx.set_xlabel("x, [mm]", labelpad=-33)
        self.fAx.set_ylabel("y, [mm]")
        reflCanvas = FigureCanvas(self.fFig)
        self.sw1.add_with_viewport(reflCanvas)

        self.rFig = plt.figure(figsize=(10, 10), dpi=100, facecolor="white")
        self.rAx = self.rFig.add_subplot(111)
        self.rAx.set_xlabel("Wavelength, [A]", labelpad=-33)
        self.rAx.set_ylabel("Reflectivity, %")
        reflCanvas = FigureCanvas(self.rFig)
        self.sw3.add_with_viewport(reflCanvas)

        self.xResFig = plt.figure(figsize=(10, 10), dpi=100, facecolor="white")
        self.xResAx = self.xResFig.add_subplot(111)
        self.xResAx.set_xlabel("x, [mm]", labelpad=-33)
        self.xResAx.set_ylabel("Counts, [n]")
        reflCanvas = FigureCanvas(self.xResFig)
        self.xRes.add_with_viewport(reflCanvas)

        #    
        self.wResAx = self.xResAx.twiny()

        self.wResAx.set_xlabel("Wavelength, [A]", labelpad=-33)
        #


        self.resOut = resPlot(self.fAx, self.fFig,
                              self.mAx, self.mFig,
                              self.rAx, self.rFig,
                              self.wResAx, self.xResAx, self.xResFig, self.outBuf)

        self.sys.resPlot = self.resOut

        self.win.show_all()

        Gtk.main()


if __name__ == '__main__':
    s = spGeneral()
    s.initWindow()
