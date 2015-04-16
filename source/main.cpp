#include "./headers/main.hpp"

int main(int argc, char** argv){

	

	if (argc < 2) {
		std::cout << "Invlaid number of inputs: " << argc << std::endl;
		return -1;
	}

	srand (time(NULL));

	std::vector<unsigned int> topology;
	topology.push_back(400);// Input nodes
	topology.push_back(100);
	topology.push_back(62);// Output nodes 
	Net nnet(topology);

	std::map<char, std::string> symbolmap;
	load_symbolmap(symbolmap, "letter_values.txt");

	if (argv[1][0] == 'g') {
		std::cout << "Generating training data from SD19 data set, this could take a while..." << std::endl;
		generate_training_data_SD19(topology, "training_data.txt", symbolmap);
		std::cout << "Training data generated!" << std::endl;
	}
	else if (argv[1][0] == 't'){
		std::cout<<"Restoring weights..."<<std::endl;
		try {
			nnet.load_weights("weights.txt");
			std::cout<<"Weights restored, commencing training!"<<std::endl;
		}
		catch(...) {
			std::cout << "Failed to load weights" << std::endl;
		}
		
		for (int i = 0; i < 1000000; i++) {
			for (int j = 0; j < 1; j++) {
				train_network("training_data.txt", nnet);
			}
			nnet.store_weights("weights.txt");
			std::cout<<"Recent average error: " << nnet.getRecentAverageError() << std::endl;
			if (nnet.getRecentAverageError() < 0.0005) {
				break;
			}
		}
	}
	else if(argv[1][0] == 'c') {
		if (argc < 3) {
			std::cout << "Invalid number of canny inputs: " << argc << std::endl;
			return -1;
		}
		clock_t t1,t2,t3,t4;
		t1 = clock();

		CImg<unsigned char> image(argv[2]);
		rescale_image(image);
		
	    t2=clock();
	    std::cout<<"Runtime of image loading and resizing: "<<((float)t2-(float)t1)/CLOCKS_PER_SEC<<std::endl;
	    CImg<unsigned char> edges(image.width(), image.height(), image.depth(), 1);
		run_canny_edge_detection(image, edges, CImgconsts::GAUSSIAN_SIZE, CImgconsts::GAUSSIAN_SIGMA, CImgconsts::HIGH_THRESHOLD_SCALE);

		t3=clock();
		std::cout<<"Runtime of canny edge detector: "<<((float)t3-(float)t2)/CLOCKS_PER_SEC<<std::endl;

		std::vector<int> vertical_line_indexes;

		std::cout<<"Restoring weights..."<<std::endl;
		nnet.load_weights("weights.txt");
		std::cout<<"Weights restored, commencing reading!"<<std::endl;

		crop_empty_space(edges, 255, 0);
		create_grid_separation(edges, vertical_line_indexes, 155, 255, 0);

		segment_letters(edges, vertical_line_indexes, 155);
		//read_letters(edges, vertical_line_indexes, 155, nnet, topology, "read_text.txt");

		t4=clock();
		std::cout<<"Runtime of image segmentation and reading: "<<((float)t4-(float)t3)/CLOCKS_PER_SEC<<std::endl;
		std::cout<<"Runtime in total: "<<((float)t4-(float)t1)/CLOCKS_PER_SEC<<std::endl;

		(image, edges).display("Edge detection", false);
	}
	else if(argv[1][0] == 'h') {
		if (argc < 3) {
			std::cout << "Invalid number of bottom hat inputs: " << argc << std::endl;
			return -1;
		}
		
		clock_t t1,t2,t3,t4;
		t1 = clock();

		CImg<unsigned char> image(argv[2]);
		rescale_image(image);
	    t2=clock();
	    std::cout<<"Runtime of image loading and resizing: "<<((float)t2-(float)t1)/CLOCKS_PER_SEC<<std::endl;

	    CImg<unsigned char> grayscale(image.width(), image.height(), image.depth(), 1);
	    convert_to_greyscale(image, grayscale);
	 	STRMask mask("tophatmask.txt");
	    bottom_hat_trans(grayscale, mask);
	    convert_to_binary(grayscale, return_otsu_threshold(grayscale));
	    t3=clock();
	    std::cout<<"Runtime of bottom hat transform: "<<((float)t3-(float)t2)/CLOCKS_PER_SEC<<std::endl;

	    std::vector<int> vertical_line_indexes;

	    crop_empty_space(grayscale, 255, 0);
	    create_grid_separation(grayscale, vertical_line_indexes, 155, 255, 0);
		segment_letters(grayscale, vertical_line_indexes, 155);
		//read_letters(grayscale, vertical_line_indexes, 155, nnet, topology, "read_text.txt");
		t4=clock();

		std::cout<<"Runtime of image segmentation and reading: "<<((float)t4-(float)t3)/CLOCKS_PER_SEC<<std::endl;
		std::cout<<"Runtime in total: "<<((float)t4-(float)t1)/CLOCKS_PER_SEC<<std::endl;
	}
	else {
		std::cout << "Invalid input!" << std::endl;
	}
}


/*	
//TEST NETWORK AS A XOR GATE
int main(int argc, char** argv) {
	std::vector<unsigned int> topology;
	topology.push_back(2);// Input nodes
	topology.push_back(4);// Hidden nodes
	topology.push_back(1);// Output nodes, all ascii symbols plus norwegian letters 
	Net nnet(topology);

	std::cout<<"Restoring weights..."<<std::endl;
	nnet.load_weights("xor_weights.txt");
	std::cout<<"Weights restored, commencing training!"<<std::endl;
	for (int i = 0; i < 8000; i++) {
		train_network("xor_data.txt", nnet);
		nnet.store_weights("xor_weights.txt");
		std::cout<<"Recent average error: " << nnet.getRecentAverageError() << std::endl;
		if (nnet.getRecentAverageError() < 0.000000000001) {
			break;
		}
	}
	std::cout << '\n';

	std::vector<double> input_values;
	std::vector<double> result_values;

	input_values.push_back(-0.5);
	input_values.push_back(-0.5);

	nnet.feedForward(input_values);
	nnet.getResults(result_values);
	std::cout<<"Recent average error: " << nnet.getRecentAverageError() << std::endl;
	for (auto it = result_values.begin(); it != result_values.end(); it++) {
		std::cout << *it << ' ';
	}
	std::cout << '\n';
}*/