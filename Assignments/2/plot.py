#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import os
import pandas as pd
import collections as coll
import matplotlib.pyplot as plt
# from scipy import stats
def add_value_labels(ax, spacing=5):
    """Add labels to the end of each bar in a bar chart.

    Arguments:
        ax (matplotlib.axes.Axes): The matplotlib object containing the axes
            of the plot to annotate.
        spacing (int): The distance between the labels and the bars.
    """

    # For each bar: Place a label
    for rect in ax.patches:
        # Get X and Y placement of label from rect.
        y_value = rect.get_height()
        x_value = rect.get_x() + rect.get_width() / 2

        # Number of points between bar and label. Change to your liking.
        space = spacing
        # Vertical alignment for positive values
        va = 'bottom'

        # If value of bar is negative: Place label below bar
        if y_value < 0:
            # Invert space to place label below
            space *= -1
            # Vertically align label at top
            va = 'top'

        # Use Y value as label and format number with one decimal place
        label = "{:.3f}".format(y_value)

        # Create annotation
        ax.annotate(
            label,                      # Use `label` as label
            (x_value, y_value),         # Place label at end of the bar
            xytext=(0, space),          # Vertically shift label by `space`
            textcoords="offset points", # Interpret `xytext` as offset in points
            ha='center',                # Horizontally center label
            va=va, rotation="vertical")                      # Vertically align label differently for
                                        # positive and negative values.
mix_col_rename={' ef=0.1':"ef=0.1", ' ef=0.2':'ef=0.2', ' ef=0.3':'ef=0.3', ' ef=0.4':'ef=0.4', ' ef=0.5':'ef=0.5', 'ef= 0.5':'ef=0.5', ' Real':'Real', ' FiFo':'Fifo'}
mix_rename_dict={
1 : "mix1",
2 : "mix2",
3 : "mix3",
4 : "mix4",
5 : "mix5",
6 : "mix6",
7 : "mix7",
8 : "mix8",
9 : "mix9",
10 : "mix10",
11 : "mix11",
12 : "mix12",
13 : "mix13",
14 : "mix14",
15 : "mix15"
}
if_speedup=0

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Invalid list:")
        print("1: file listing algos to analyse")
        # print("2: denom file name , if any, else enter NA")
        print("2: output folder dir")
        print("3: sorting according to results required or not: 0 or 1")
        print("4: updatig the list for algo analysis: 0 if no, for which algo updation corresponds to which number see code")
        # print("4: Title of the graph formed")
        sys.exit(0)

    file_name=sys.argv[1]
    # remove rhe last file name to get the location of file
    file_folder="/".join(file_name.split('/')[0:len(file_name.split('/'))-1])
    # denom_file=sys.argv[2]
    output_folder=sys.argv[2]
    sort=int(sys.argv[3])
    updateAlgosforPlot=int(sys.argv[4])
    if(not(sort==0 or sort==1)):
        print("invalid sort argument: put 0 for no 1 for yes")
        sys.exit(0)
    if not os.path.exists(output_folder):
        os.mkdir(output_folder)
    output_loc=output_folder+"/"
    # if denom_file != "NA":
    #     if_speedup=1
    #print(if_speedup)

    file = open(file_name)

    algos_list=[]
    for line in file:
        temp=line.split('\n')

        # if denom_file == temp [0]:
        #     continue;
        algos_list.append(temp[0])

    # print(algos_list)

    #read csv each file wise

    count=0;

    combined=[]
    i=0
    dataDict={}
    graphNames=[]
    algoforPlot =[]
    for csv_fileW in algos_list:
        temp=csv_fileW.split()
        # assuming that the first word of the line contains the csv file location wrt the location of the file containnbg algo for analysis
        csv_file=temp[0]
        csv_file1=csv_file
        csv_file=file_folder+"/"+csv_file
        if not os.path.exists(csv_file):
            print(str(csv_file)+" File does not exist")
        else:
            temp1=csv_file1.split("/")
            algoName=temp1[0].replace("inpF","")
            algoforPlot.append(algoName)
            # assuming that in the csv file location first one contains the experiment name and the last contains the csv file. eg DQN/csv/performance.csv
            graphN=temp1[len(temp1)-1].split(".")[0]#whose graphs needs to be plotted ie performance or unfairness or ratio
            key=algoName+graphN
            # print(key)
            if graphN not in graphNames:
                graphNames.append(graphN)
            dataDict[key] = pd.read_csv(csv_file, index_col=0)
            # dataDict = dataDict.reindex(mixfile_folder_rename_dict)
            dataDict[key] = dataDict[key].rename(index = mix_rename_dict, columns=mix_col_rename)
            # print(dataDict[key])
            # i+=1
            # print(dataDict)
            # dataDict = dataDict.rename(columns = mix_col_rename)
            # print(dataDict)
    efGraphs`={}
    # print(algoforPlot)
    # print(dataDict.keys())
    # ef=["ef=0.1", "ef=0.2", "ef=0.3", "ef=0.4", "ef=0.5"]
    ef=[ "ef=0.4"]
    for graphN in graphNames:
        efGraphs[graphN]={}
        for i in ef:
            efGraphs[graphN][i]={}
            efGraphs[graphN][i]['Mean']={}
            # for mix in
    for key in dataDict.keys():
        for al in algoforPlot:
            if al in key:
                for graphN in graphNames:
                    if graphN in key:
                        for i in ef:
                            sum=0
                            sumF=0
                            sumR=0
                            for mix in dataDict[key].index:
                                if not mix in efGraphs[graphN][i].keys():
                                    efGraphs[graphN][i][mix]={}
                                # print(mix+i)
                                # print(dataDict[key].loc[mix,i])
                                sum+=dataDict[key].loc[mix,i]
                                efGraphs[graphN][i][mix][al]=dataDict[key].loc[mix,i]
                                sumR+=dataDict[key].loc[mix,'Real']
                                efGraphs[graphN][i][mix]['Real']=dataDict[key].loc[mix,'Real']
                                sumF+=dataDict[key].loc[mix,'Fifo']
                                efGraphs[graphN][i][mix]['Fifo']=dataDict[key].loc[mix,'Fifo']
                            lenMix=len(dataDict[key].index)
                            # print(sum)
                            # lenMix=len(dataDict[key].keys())
                            mean=sum/lenMix
                            meanR=sumR/lenMix
                            meanF=sumF/lenMix
                            efGraphs[graphN][i]['Mean'][al]=mean
                            efGraphs[graphN][i]['Mean']['Real']=meanR
                            efGraphs[graphN][i]['Mean']['Fifo']=meanF
    #for plotting exploration factor wise graphs
    # for dd in efGraphs.keys():
    #     for k in efGraphs[dd].keys():
    #         val=efGraphs[dd][k]
    #         fig, ax = plt.subplots()
    #         df=pd.DataFrame(val)
    #         plot=df.T.plot(y=df.T.columns, kind="bar", ax=ax, width =0.85, figsize=(18,10))
    #         add_value_labels(ax)
    #         plt.ylabel(dd)
    #         plt.xlabel("Mixes")
    #         plt.title(dd + " vs mixes" )
    #         ax.legend( loc= 8 )
    #         figName=dd+k
    #         output_loc1=output_loc+dd+"/"
    #         if not os.path.exists(output_loc1):
    #             os.mkdir(output_loc1)
    #         fig.savefig(output_loc1+figName+".png", dpi=500,  bbox_inches='tight')
    #         # plt.show()
    #         plt.close(fig)
    #         # break
    #     # break
    algoforPlotUpdated={}

    for e in ef:
        realMeanPer=efGraphs["performance"][e]['Mean']['Real']
        realMeanUnfairness=efGraphs["unfairness"][e]['Mean']['Real']
        algoforPlotUpdated[e]=[]
        if updateAlgosforPlot==1:#only print those algo with performance more than real and unfairness less that real
            for al in algoforPlot:
                if(efGraphs["performance"][e]['Mean'][al] > realMeanPer and efGraphs["unfairness"][e]['Mean'][al] < realMeanUnfairness ):
                        algoforPlotUpdated[e].append(al)

    #for plotting the mean over all exploration values and all mixes
    for dd in efGraphs.keys():
        mean={}
        mean['Real']={}
        mean['Fifo']={}
        for al in algoforPlot:
            mean[al]={}
            for e in ef:
                if (al in algoforPlotUpdated[e] and updateAlgosforPlot!=0) or updateAlgosforPlot==0 :
                    mean[al][e]=efGraphs[dd][e]['Mean'][al]
                mean['Real'][e]=efGraphs[dd][e]['Mean']['Real']
                mean['Fifo'][e]=efGraphs[dd][e]['Mean']['Fifo']
        # for al in algoforPlot:
        #     e="ef=0.4"
        #     mean[al]=efGraphs[dd][e]['Mean'][al]
        #     mean['Real']=efGraphs[dd][e]['Mean']['Real']
        #     mean['Fifo']=efGraphs[dd][e]['Mean']['Fifo']
        #             # counter.update(mean[e])
        # meanS=sorted(mean.items(), key=lambda x: x[1]["ef=0.4"])
        # sum=dict(counter)
        # l_ef=(len(ef))
        # meanOfMean={}
        # for k in sum.keys():
        #     meanOfMean[k]=sum[k]/l_ef
        # # print(dd)
        # # print((counter))
        # # print((meanOfMean))
        #
        # mean['mean']=meanOfMean
        fig, ax = plt.subplots()
        df=pd.DataFrame(mean)
        dfT=df.T
        dfT=dfT.dropna()
        if(sort==1):
            dfT=dfT.sort_values('ef=0.4')
        plot=dfT.plot(y=df.T.columns, kind="bar", ax=ax, figsize=(18,10))
        add_value_labels(ax)
        plt.ylabel(dd)
        # plt.xlabel("Mixes")
        plt.title(dd  )
        # plt.title(dd + " vs mixes" )
        ax.legend( loc= 8 )
        figName=dd+"Mean"
        if(sort==1):
            figName+="Sorted"
        if(updateAlgosforPlot!=0):
            figName+="Better"
        output_loc1=output_loc
        if not os.path.exists(output_loc1):
            os.mkdir(output_loc1)
        fig.savefig(output_loc1+figName+".png", dpi=500,  bbox_inches='tight')
        # plt.show()
        plt.close(fig)
