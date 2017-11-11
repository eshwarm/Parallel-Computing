# -*- coding: utf-8 -*-
"""
Created on Mon Oct  2 18:00:51 2017

@author: Eshwar
"""
import os
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter, defaultdict
cwd = os.getcwd()


"""
This code takes time taken to execute chunk from the result generated
and plot the scheduler graph
"""


# Read data from file into variables
name = 0
n = 0
intensity =0
sync = 'thread'
granularity = 0
for root, dirs, filenames in os.walk(cwd):
        f = 'gantt_100000_1000_8_thread_100'
        if 'gantt_1' in f:
            names  = f.split('_')
            name = names[0]
            n = names[1]
            intensity =names[2]
            no_threads = names[3]
            sync = names[4]
            granularity = names[5]
            thread_id, start, range_ = np.loadtxt(f, unpack=True)
            start_list = start.tolist()
            range_list = range_.tolist()
            unique_threads = set(thread_id.tolist())
            thread_list =thread_id.tolist()
            count =10
            threads = []
            thread_count = 0
            plot_range = []
            plot_range_dict = defaultdict(lambda: 0)
            unique_thread_list = []
            for uni in unique_threads:
                thread_count+=1   
                plot_range.append((count, 5))
                plot_range_dict['thread{0}'.format(thread_count)] = (count, 5)
                unique_thread_list.append('thread{0}'.format(thread_count))   
                i = 0 
                for th in thread_list:
                    if th == uni:
                        thread_list[i] = 'thread{0}'.format(thread_count)
                    i+=1
                count= count+10   
            f = map(lambda i,j,k:(i,j,k), thread_list,start_list,range_list)

            data = zip(thread_list, start_list, range_list)
            list_ =[]
            data_dict = {}
            
            for t,s,r in data:
                data_dict[t] = []
            for thread, start, range_ in list(f):
                data_dict[thread].append((start, range_))
         
            fig, ax = plt.subplots()
            for each_thread, list_ in data_dict.items():
                ax.broken_barh(list_,plot_range_dict[each_thread], facecolors='blue')
            
            ax.set_xlabel('n={0},intensity={1},no_threads={2}, sync={3}, granularity={4}'.format
                          (n,intensity,no_threads, sync, granularity))
            position = []
            for val in plot_range:
                i, j = val
                position.append((i+j/2))
            ax.set_yticks(position)
            ax.set_yticklabels(unique_thread_list)
            
            ax.grid(True)
            pdf_file = str(name) + str(n)+str(intensity)+str(threads)+ str(sync)+ str(granularity)
            fig.savefig( '{0}'.format(pdf_file) + ".pdf", format='pdf')