#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import json

def run_method(method, graph, n, k, t, iters="", kpos=""):
    if k < t: return None
    cmd = "methods/{0} {1} {2} {3} {4}".format(
            method, graph, n, k, t, iters, kpos)
    with os.popen(cmd) as f:
        line = f.readline().strip().split()
        if not line:
            return None
        return int(line[0]), int(line[1])

def predict_b(n, k, t):
    return run_method("predict", "b", n, k, t, n * n * 10000)

def predict_th(n, k, t):
    return run_method("predict", "th", n, k, t, n * n * 5000)

def predict_t(n, k, t, kpos):
    return run_method("predict", "b", n, k, t, n * n * 1000, kpos)

def proof_b(n, k, t):
    return run_method("proof", "b", n, k, t)

def proof_th(n, k, t):
    return run_method("proof", "th", n, k, t)

def proof_t(n, k, t, kpos):
    return run_method("proof", "t", n, k, t, kpos)

def eig_b(n, k, t):
    return run_method("eig", "b", n, k, t)

def eig_t(n, k, t, kpos):
    return run_method("eig", "t", n, k, t, kpos)

def eig_th(n, k, t):
    return run_method("eig", "th", n, k, t)

def not_empty(row):
    return any(True for r in row if r is not None)

def reduce_table(table, rows):
    rows = [r for r,t in zip(rows,table) if not_empty(t)]
    table = [t for t in table if not_empty(t)]
    return table, rows

def transpose(table):
    return [ [table[i][j] for i in xrange(len(table))]
            for j in xrange(len(table[0])) ]

def json_b(get_result, out_dir, caption):
    for n in xrange (7, 17):
        table = []
        rows = []
        for k in xrange(1, n-1):
            row = []
            for t in xrange(1,n-1):
                row.append(get_result(n,k,t))
                if row[-1]:
                    print n,k,t
            table.append(row)
            rows.append(k)

        table, rows = reduce_table(table, rows)
        if not len(table):
            continue
        table, cols = reduce_table(transpose(table), range(1,n-1))
        table = transpose(table)
        open("{0}/{1}.json".format(out_dir, n), "w").write(json.dumps({
            "values" : table,
            "rows" : rows,
            "cols" : ["$k \\backslash t$"] + cols,
            "caption" :
                u"{0} $\\GB:\\chi/\\alpha,n={1}$".format(caption, n)
            }))

def json_t(get_result, out_dir, caption):
    for n in xrange (6, 16):
        table = []
        rows = []
        for k in xrange(1, n-1):
            for kpos in xrange(1, k/2+1):
                row = []
                for t in xrange(1,n-1):
                    row.append(get_result(n,k,t,kpos))
                    if row[-1]:
                        print n,k,t,kpos

                rows.append("{0}, {1}".format(k-kpos, kpos))
                table.append(row)

        table, rows = reduce_table(table, rows)
        if not len(table):
            continue
        table, cols = reduce_table(transpose(table), range(1,n-1))
        table = transpose(table)

        open("{0}/{1}.json".format(out_dir, n), "w").write(json.dumps({
            "values" : table,
            "rows" : rows,
            "cols" : ["$k_{-1},k_1 \\backslash t$"] + cols,
            "caption" :
                u"{0} $\\GT:\\chi/\\alpha,n={1}$".format(caption, n)
            }))

def json_th(get_result, out_dir, caption):
    for n in xrange (6, 17):
        table = []
        for k in xrange(1, n-1):
            row = []
            for t in xrange(0,n-1):
                row.append(get_result(n,k,t))
                if row[-1]:
                    print n,k,t

            table.append(row)

        table, rows = reduce_table(table, range(1, n-1))
        if not len(table):
            continue
        table, cols = reduce_table(transpose(table), range(0,n-1))
        table = transpose(table)

        open("{0}/{1}.json".format(out_dir, n), "w").write(json.dumps({
            "values" : table,
            "rows" : rows,
            "cols" : ["$k \\backslash t$"] + cols,
            "caption" :
                u"{0} $\\GTH:\\chi/\\alpha,n={1}$".format(caption, n)
            }))


if __name__ == "__main__":
    eig_caption = u"Оценка через $\\lambda_{min}$"
    eig_dir = "data/eig/"
    json_b(eig_b,  eig_dir + "b", eig_caption)
    json_t(eig_t,  eig_dir + "t", eig_caption)
    json_th(eig_th,  eig_dir + "th", eig_caption)

    predict_caption = u"Предположительная оценка"
    predict_dir = "data/predict/"
    json_b(predict_b, predict_dir + "b", predict_caption)
    json_t(predict_t, predict_dir + "t", predict_caption)
    json_th(predict_th, predict_dir + "th", predict_caption)

    proof_caption = u"Точное значение $\alpha$ для"
    proof_dir = "data/proof/"
    json_b(proof_b, proof_dir + "b", proof_caption)
    json_t(proof_t, proof_dir + "t", proof_caption)
    json_th(proof_th, proof_dir + "th", proof_caption)


