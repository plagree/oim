This folder contains the source code for the Online Influence Maximization with
Persistence algorithms, as described in this [paper][1] and the model-free
[extension][6]. The source code is header-only.

# Compiling

The *Makefile* is in the main folder, so simply execute *make*:

    make clean; make

The output binary is *oim*.

## Dependencies

The *Makefile* requires GCC 4.9.0 (or superior) and uses C++14 features.

The code needs the Boost C++ library headers. It assumes the include files are
present in */usr/local/include*. If your Boost installation is someplace else,
you have to modify the *INCLUDE_DIRS* directive in *Makefile*. The binary
library does not need to be linked.

# Datasets

The networks used in the experimental section of [our paper][6] are available
[here](https://drive.google.com/open?id=0B6peHJJcM5GvQnZGdHU1U1owa0U). We
provide scripts to generate your own datasets in the `graph/` folder.

# Methods and usage

The program expects as input a tab delimited graph file of the following format:

    node1 <TAB> node2 <TAB> prob

where *node1* and *node2* are the endpoints of a graph edge, and *prob* is the
influence probability.

The following methods are currently supported:

1. *exponentiated gradient*, which is run as follows:

        ./oim --eg <graph> <alpha> <beta> <exploit> <trials> <k> [<model>
        <update> <update_type> <cascades>]

2. *missing mass*, which runs as follows:

        ./oim --missing_mass <graph> <policy> <reduction> <budget> <k>
        <n_experts> [<model> <cascades>]

3. *real graph*, which executes on the real graph:

        ./oim --real <graph> <exploit> <trials> <k> [<model> <samples>
        <cascades>]

## Parameters

The parameters are set as follows:

* *graph* is the name of the graph file
* *alpha*, *beta* are the global prior on the edges of the graph
* *exploit*, *explore* can take any of the following values: **0** Random,
  **1** AdaptiveDegree, **2** Maxdegree, **3** [CELF][2], **4** [TIM][3],
  **5** [SSA][4], **6** [PMC][5]
* *samples* is the number of spreads to estimate the expected value of chosen
  seeds
* *trials* is the number of trials, *k* is the number of seeds in each trial
* *update* is **1** if the graph is updated, **0** otherwise
* *update_type* is the type of update: **0** local only, **1** least squares or
  **2** maximum likelihood
* *reduction* can take the following values: **0** max cover, **1** highest
  degree
* *policy* can take the following values: **0** random, **1** Good-UCB
* *model* can take the following values: **0** Linear Threshold, **1**
  Independent Cascade
* *cascades* contains the path to the file containing **real** cascades (logs)

## Output

The different methods write on the standard output with the following format:

1. *exponentiated gradient*:

        stage <TAB> cum spread <TAB> expected spread <TAB> tselection <TAB> tupdate <TAB>
        tround <TAB> ttotal <TAB> theta <TAB> memory <TAB> k <TAB> model <TAB> seeds

2. *missing mass*:

        stage <TAB> cumulative spread <TAB> treduction <TAB> tselection <TAB> tupdate <TAB>
        tround <TAB> ttotal <TAB> memory <TAB> k <TAB> n_experts <TAB> n_policy <TAB>
        n_reduction <TAB> model <TAB> seeds

3. *real graph*:

        stage <TAB> cumulative spread <TAB> expected spread <TAB> tround <TAB>
        ttotal <TAB> k <TAB> model <TAB> seeds

# Contributors

* Paul Lagrée (Université Paris-Sud)
* Siyu Lei (University of Hong Kong) on code published in 2015
* Silviu Maniu (University of Hong-Kong, then Université Paris-Sud)
* Luyi Mo (University of Hoong-Kong) on code published in 2015

# License

The source code is provided as-is under an MIT License. If it is useful to you,
please cite the seminal [paper][1] and the model-free [extension][6].

[1]: <http://arxiv.org/pdf/1506.01188v1.pdf> "S. Lei, S. Maniu, L. Mo, R. Cheng, P. Senellart. Online Influence Maximization. KDD 2015"

[2]: <http://snap.stanford.edu/class/cs224w-readings/goyal11celf.pdf> "A. Goyal, W. Lu, L. Lakshmanan. CELF++: Optimizing the Greedy Algorithm for Influence Maximization in Social Networks. WWW 2011"

[3]: <http://arxiv.org/pdf/1404.0900v2.pdf> "Y. Tang, X. Xiao, and Y. Shi. Influence maximization: Near-optimal time complexity meets practical efficiency. SIGMOD 2014"

[4]: <https://arxiv.org/pdf/1605.07990v2.pdf> "H. T. Nguyen, M. T. Thai, and T. N. Dinh. Stop-and-Stare: Optimal Sampling Algorithms for Viral Marketing in Billion-scale Networks. SIGMOD 2016"

[5]: <https://www.aaai.org/ocs/index.php/AAAI/AAAI14/paper/download/8455/8411> "N. Ohsaka, T. Akiba, Y. Yoshida and K. Kawarabayashi. Fast and Accurate Influence Maximization on Large Networks with Pruned Monte-Carlo Simulations. AAAI 2014"

[6]: <http://arxiv.org/pdf/1702.05354.pdf> "P. Lagrée, O. Cappé, B. Cautis, S. Maniu. Online Model-Free Influence Maximization with Persistence."
