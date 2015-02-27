#include "./headers/main.hpp"



int main(int argc, char** argv){

	/*Initializes SDL for graphical display*/
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		SDLerrorLogger("SDL initialization");
		std::cout<<"Failed to initialize SDL, see errorlog for details."<<std::endl;
		return 1;
	}

	/*Disables pesky screensavers while our wonderful graphics are beeing displayed*/
	SDL_DisableScreenSaver();

	/*Initializes a window to render graphics in*//*
	SDL_Window *win = SDL_CreateWindow("EM", 0, 0, SDLconsts::window_length, SDLconsts::window_height, 0);
	if (win == nullptr){
		SDLerrorLogger("SDL_CreateWindow");
		std::cout<<"Failed to create SDL window, see errorlog for details."<<std::endl;
		return 1;
	}*/

	/*Initializes the renderer to draw in*//*
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDLerrorLogger("SDL_CreateRenderer");
		std::cout<<"Failed to create SDL renderer, see errorlog for details."<<std::endl;
		return 1;
	}*/

	CImg<double> image(argv[1]);
	CImg<double> grayscale1(image.width(), image.height(), image.depth(), 1);
	CImg<double> grayscale2(image.width(), image.height(), image.depth(), 1);
	CImg<double> edges(image.width(), image.height(), image.depth(), 1);
	CImg<unsigned char> direction(image.width(), image.height(), image.depth(), 1);
	CImg<double> magnitude(image.width(), image.height(), image.depth(), 1);
	convert_to_greyscale(image, grayscale1);
	convert_to_greyscale(image, grayscale2);

	for(int i = 0; i < ((int)argv[2][0] - '0'); i++){
		apply_gaussian_smoothing(grayscale2);
	}


	calculate_gradient_magnitude_and_direction(grayscale2, 
												direction, 
												magnitude);

	apply_non_maximum_suppress(grayscale2, 
								direction, 
								magnitude);

	hysteresis(edges, grayscale2, get_high_threshold(grayscale2));
	
	grayscale1.save("grayscale1.bmp");
	grayscale2.save("grayscale2.bmp");
	magnitude.save("magnitude.bmp");
	direction.save("edges.bmp");
		
	(image, grayscale1, magnitude, edges).display("RGB to Grayscale");

	waitForEvent();
	
}

void hysteresis(CImg<double>& edges, CImg<double>& supressed, double high_threshold){
	double low_threshold = high_threshold*CImgconsts::LOW_THRESHOLD_SCALE;
	for (int x = 1; x < supressed.width()-1; x++){
		for (int y = 1; y < supressed.height()-1; y++){
			if(supressed(x,y) > high_threshold){
				edges(x,y) = 255;
			}
			else if (supressed(x,y) < low_threshold) {
				edges(x,y) = 0;
			}
			else{
				std::set<Point> visited_pixels;
				if(check_if_a_neghbour_is_upper_threshold(x, 
														y, 
														supressed, 
														high_threshold, 
														low_threshold, 
														visited_pixels)){
					edges(x,y) = 255;
				}
				else{
					edges(x,y) = 0;
				}
			}
		}
	}
}

bool check_if_a_neghbour_is_upper_threshold(int xpos, 
											int ypos, 
											CImg<double>& supressed,
											double high_threshold, 
											double low_threshold,
											std::set<Point>& visited_pixels){
	Point this_pixel = {xpos, ypos};
	visited_pixels.insert(this_pixel);
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++){
			if(xpos+i < 2 || ypos+j < 2 || xpos+i > supressed.width()-2 || ypos+j > supressed.height()-2){
				continue;
			}
			Point current = {xpos + i, ypos + j};
			if(visited_pixels.find(current) != visited_pixels.end()){
				continue;
			}
			else if(supressed(xpos + i, ypos + j) > high_threshold){
				return true;
			}
			else if (supressed(xpos + i, ypos + j) < low_threshold){
				visited_pixels.insert(current);
				continue;
			}
			else{
				if(check_if_a_neghbour_is_upper_threshold(xpos + i, 
														ypos + j, 
														supressed, 
														high_threshold, 
														low_threshold, 
														visited_pixels)){
					return true;
				}
			}
		}
	}
	return false;
}

double get_high_threshold(CImg<double>& supressed){
	double high_threshold = 0;
	for (int x = 1; x < supressed.width()-1; x++){
		for (int y = 1; y < supressed.height()-1; y++){
			if(supressed(x,y) > high_threshold){
				high_threshold = supressed(x,y);
			}
		}
	}
	return high_threshold * CImgconsts::HIGH_THRESHOLD_SCALE;
}

void apply_non_maximum_suppress(CImg<double>& grayimage, 
								CImg<unsigned char>& direction, 
								CImg<double>& magnitude){

		for (int x = 1; x < grayimage.width()-1; x++){
			for (int y = 1; y < grayimage.height()-1; y++){
				switch(direction(x,y)){
					case 0:
						if(magnitude(x+1,y) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 10:
						if(magnitude(x+1,y-1) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 20:
						if(magnitude(x,y-1) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 30:
						if(magnitude(x-1,y-1) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 40:
						if(magnitude(x-1,y) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 50:
						if(magnitude(x-1,y+1) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 60:
						if(magnitude(x,y+1) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 70:
						if(magnitude(x+1,y+1) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
					case 80:
						if(magnitude(x+1,y) >= magnitude(x,y)){
							grayimage(x,y) = 0;
						}
						else{
							grayimage(x,y) = magnitude(x,y);
						}
						break;
				}
			}
		}
}

void calculate_gradient_magnitude_and_direction(CImg<double>& grayimage, 
												CImg<unsigned char>& direction, 
												CImg<double>& magnitude) {

	CImg<double> dx(grayimage.width(), grayimage.height(), grayimage.depth(), 1);
	CImg<double> dy(grayimage.width(), grayimage.height(), grayimage.depth(), 1);
	double aproxximate_direction = 0.0;
	for (int x = 1; x < grayimage.width()-1; x++){
		for (int y = 1; y < grayimage.height()-1; y++){
			dx(x,y) = (grayimage(x+1, y-1 ) * CImgconsts::GX[2][0] + grayimage(x-1, y-1 ) * CImgconsts::GX[0][0] + 
						grayimage(x+1, y ) * CImgconsts::GX[2][1] + grayimage(x-1, y ) * CImgconsts::GX[0][1] + 
						grayimage(x+1, y+1 ) * CImgconsts::GX[2][2] + grayimage(x-1, y+1 ) * CImgconsts::GX[0][2]);

			dy(x,y) = (grayimage(x+1, y-1 ) * CImgconsts::GX[0][2] + grayimage(x-1, y-1 ) * CImgconsts::GX[0][0] + 
						grayimage(x+1, y ) * CImgconsts::GX[1][2] + grayimage(x-1, y ) * CImgconsts::GX[1][0] + 
						grayimage(x+1, y+1 ) * CImgconsts::GX[2][2] + grayimage(x-1, y+1 ) * CImgconsts::GX[2][0]);

			magnitude(x,y) = sqrt((dx(x,y)*dx(x,y)) + (dy(x,y)*dy(x,y)));


			// [0, 45), [45, 90), [90, 135), [135,180), [180, 225), [225, 270), [270, 315), [315, 360)
			// [337, 22), [22, 67), [67, 112), [12,157), [157, 202), [202, 247), 
			//[247, 292), [292, 337)
			// [89.82, 87.39), [--, 89.14), [--, 89.48), [--, 89.63), [--, 89.71), [--, 89.76), 
			//[--, 89.80), [--, 89.82)

			//in radians: [1.567654, 1.525243), [--, 1.555786), [--, 1.561720), [--, 1.564338), [--, 1.565734), [--, 1.566607), 
			//[--, 1.567305), [--, 1.567654)
			if (dy(x,y) == 0) {
				aproxximate_direction = 0.0;
			}
			else if (dx(x,y) == 0) {
				aproxximate_direction = atan(dy(x,y)/0.1);
			}
			else{
				aproxximate_direction = atan(dy(x,y)/dx(x,y));
			}

			if(aproxximate_direction < 1.525243) {
				direction(x,y) = 0;
			}
			else if(aproxximate_direction >= 1.525243 && aproxximate_direction < 1.555786) {
				direction(x,y) = 10;//45
			}
			else if(aproxximate_direction >= 1.555786 && aproxximate_direction < 1.561720) {
				direction(x,y) = 20;//90
			}
			else if(aproxximate_direction >= 1.561720 && aproxximate_direction < 1.564338) {
				direction(x,y) = 30;//135
			}
			else if(aproxximate_direction >= 1.564338 && aproxximate_direction < 1.565734) {
				direction(x,y) = 40;//180
			}
			else if(aproxximate_direction >= 1.565734 && aproxximate_direction < 1.566607) {
				direction(x,y) = 50;//225
			}
			else if(aproxximate_direction >= 1.566607 && aproxximate_direction < 1.567305) {
				direction(x,y) = 60;//270
			}
			else if(aproxximate_direction >= 1.567305 && aproxximate_direction < 1.567654) {
				direction(x,y) = 70;//315
			}
			else if(aproxximate_direction >= 1.567654) {
				direction(x,y) = 80;//360
			}
		}
	}
}

void apply_gaussian_smoothing(CImg<double>& grayimage) {

	float gaussian_sum = 0.0;

	for (int x = 0; x < CImgconsts::GAUSSIAN_SIZE; x++) {
		for (int y = 0; y < CImgconsts::GAUSSIAN_SIZE; y++) {
			gaussian_sum += CImgconsts::GAUSSIAN[x][y];
		} 
	}

	float pixel_sum = 0.0;

	for (int i = 0; i < grayimage.width(); i++){
		for (int j = 0; j < grayimage.height(); j++){
			pixel_sum = 0.0;
			for (int k = -CImgconsts::GAUSSIAN_OFFSET_FROM_CENTER; k <= CImgconsts::GAUSSIAN_OFFSET_FROM_CENTER; k++){
				for (int l = -CImgconsts::GAUSSIAN_OFFSET_FROM_CENTER; l <= CImgconsts::GAUSSIAN_OFFSET_FROM_CENTER; l++) {
					if (i+k > grayimage.width() || i+k < 0 || j+l > grayimage.height() || j+l < 0) {
						continue;
					}
					else{
						pixel_sum += (grayimage(i + k, j + l) * 
							CImgconsts::GAUSSIAN[k + CImgconsts::GAUSSIAN_OFFSET_FROM_CENTER]
												[l + CImgconsts::GAUSSIAN_OFFSET_FROM_CENTER]);
					}
				}
			}
			grayimage(i, j) = pixel_sum/gaussian_sum;
		}
	}
}

/* Converts an image to grayscale using the luminocity method, it puts extra weight on green */
void convert_to_greyscale(CImg<double>& image, CImg<double>& gray){
	double red, green, blue;
	for (int x = 0; x < image.width(); x++){
		for (int y = 0; y < image.height(); y++){
			red = image(x,y,0, 0);
			green = image(x,y,0, 1);
			blue = image(x,y,0, 2);
			gray(x,y) = round((red * CImgconsts::RED_SCALE + 
				green * CImgconsts::GREEN_SCALE + 
				blue * CImgconsts::BLUE_SCALE)/3.0);
		}
	}
}

void pollevent(bool& var){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch (event.type){
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_KEYDOWN:
				var = true;
				break;
			default:
				break;
		}
	}
}


void waitForEvent(){
	SDL_Event event;
	bool done = false;
	while((!done) && (SDL_WaitEvent(&event))) {
        switch(event.type) {
    
            case SDL_KEYDOWN:
                done = true;
                break;


            case SDL_QUIT:
                done = true;
                exit(0);
                break;
                
            default:
                break;
        } 
            
    }
}
