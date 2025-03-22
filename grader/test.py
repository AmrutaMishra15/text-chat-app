import matplotlib.pyplot as plt
import pandas as pd
import subprocess
import os

def run_experiment(messages, corruption, time, loss, window, path, output_file):
    """
    Run the network experiment using specified parameters.

    Args:
        messages (int): The number of messages to send.
        corruption (float): The probability of message corruption.
        time (int): The arrival time units for messages.
        loss (float): Probability of message loss.
        window (int): Window size used by the protocol.
        path (str): Path to the protocol implementation.
        output_file (str): File to output experiment results.

    TODO:
        - Build the command line string to run the experiment.
        - Print and execute the command.
    """
    abs_path = "./../pa2/" + path 
    print(abs_path)
    # Replace the following line with your implementation.
    command = "./run_experiments  -m={} " \
              "-l={} -c={} -t={} " \
              "-w={} -p={} -o={}".format(messages, loss, corruption, time, window, abs_path, output_file)
#     cmd = "./run_experiments -p {} -o {} -m {} -t {} -c {} -l {} -w {}".format(
#         path, output_file, messages, time, corruption, loss, window
#     )
    # command = "../pa2/{} -s 1 -w {} -m {} -l {} -c {} -t {} -v 0 > {}".format(path, window, messages, loss, corruption, time, output_file)
    print("Current working directory:", os.getcwd())
    print("Executing command:", command)
    
    subprocess.run(command, shell=True)

def plot_and_save_data(data, x_label, y_label, title, graph_filename, data_filename):
    """
    Plot the experiment data and save the plot and data to files.

    Args:
        data (dict): Dictionary containing protocol data with x and y values.
        x_label (str): Label for the x-axis.
        y_label (str): Label for the y-axis.
        title (str): Title of the graph.
        graph_filename (str): Filename for the saved graph.
        data_filename (str): Filename for the saved data in CSV format.

    TODO:
        - Create a plot using matplotlib.
        - Plot each protocol's data on the graph.
        - Save the plot to 'graph_filename'.
        - Save the data to a CSV file named 'data_filename'.
    """
    # Replace the following lines with your implementation.
    plt.figure()
    for protocol, vals in data.items():
        plt.plot(vals['x'], vals['y'], label=protocol)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend()
    plt.savefig(graph_filename)
    plt.close()
    
    # Convert data dictionary to DataFrame for CSV saving
    df = pd.DataFrame.from_dict({(i,j): data[i][j] 
                                 for i in data.keys() 
                                 for j in data[i].keys()},
                                orient='index')
    df.to_csv(data_filename)

def get_average_throughput(output_file):
    """
    Calculate the average throughput from an output file.

    Args:
        output_file (str): The path to the CSV output file of the experiment.

    Returns:
        float: The average throughput or 0 if an error occurs.

    TODO:
        - Read the output file using pandas.
        - Calculate and return the average throughput.
    """
    # Replace the following lines with your implementation.
    try:
        df = pd.read_csv(output_file)  # Skip the first row containing headers
        throughputs = df['Throughput'].tolist()
        return sum(throughputs) / len(throughputs)
    except Exception as e:
        print("Error reading {}: {}".format(output_file, e))
        return 0

# Ensure the output directory exists
os.makedirs('./graphs', exist_ok=True)
os.makedirs('./outputs', exist_ok=True)

messages = 1000
corruption = 0.2
time = 50

# Experiment 1: Varying loss probability with different window sizes
protocols = ['abt', 'gbn', 'sr']
window_sizes = [10, 50]
loss_probs = [0.1, 0.2, 0.4, 0.6, 0.8]


for window_size in window_sizes:
    data = {protocol: {'x': [], 'y': []} for protocol in protocols}
    for loss_prob in loss_probs:
        for protocol in protocols:
            output_file = './outputs/exp1_{}_l{}_w{}.csv'.format(protocol, int(loss_prob * 100), window_size)
            run_experiment(messages, corruption, time, loss_prob, window_size, protocol, output_file)
            throughput = get_average_throughput(output_file)
            data[protocol]['x'].append(loss_prob)
            data[protocol]['y'].append(throughput)
    plot_and_save_data(
        data,
        x_label='Loss Probability',
        y_label='Throughput',
        title='Throughput vs Loss Probability (Window Size = {})'.format(window_size),
        graph_filename='./graphs/throughput_loss_w{}.png'.format(window_size),
        data_filename='./graphs/throughput_loss_w{}.csv'.format(window_size)
    )
            # TODO: Call `run_experiment` with appropriate arguments
            # TODO: Retrieve and process throughput data using `get_average_throughput`
            # TODO: Update `data` dictionary with throughput data
    # TODO: Call `plot_and_save_data` to plot and save the results for the current experiment

# Experiment 2: Varying window size with fixed loss probabilities
loss_probs = [0.2, 0.5, 0.8]
window_sizes = [10, 50, 100, 200, 500]

for loss_prob in loss_probs:
    data = {protocol: {'x': [], 'y': []} for protocol in protocols}
    for window_size in window_sizes:
        for protocol in protocols:
            output_file = './outputs/exp2_{}_l{}_w{}.csv'.format(protocol, int(loss_prob * 100), window_size)
            run_experiment(messages, corruption, time, loss_prob, window_size, protocol, output_file)
            throughput = get_average_throughput(output_file)
            data[protocol]['x'].append(window_size)
            data[protocol]['y'].append(throughput)
    plot_and_save_data(
        data,
        x_label='Window Size',
        y_label='Throughput',
        title='Throughput vs Window Size (Loss Probability = {})'.format(loss_prob),
        graph_filename='./graphs/throughput_window_l{}.png'.format(int(loss_prob * 100)),
        data_filename='./graphs/throughput_window_l{}.csv'.format(int(loss_prob * 100))
    )
            # TODO: Call `run_experiment` with appropriate arguments
            # TODO: Retrieve and process throughput data using `get_average_throughput`
            # TODO: Update `data` dictionary with throughput data
    # TODO: Call `plot_and_save_data` to plot and save the results for the current experiment