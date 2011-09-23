/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2010 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */

package isabel.avatar;

import java.io.*;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;

import java.util.Locale;

import javax.imageio.*;
import javax.imageio.stream.*;
import javax.imageio.plugins.jpeg.*;


class MakeAvatar {
    
     private static final Integer IMAGE_WIDTH  = 320;
     private static final Integer IMAGE_HEIGHT = 240;

     private static final Integer ICON_WIDTH  = 32;
     private static final Integer ICON_HEIGHT = 24;


    /**
     *  Scale the given image and create a RenderederedImage.
     *  The image image is scaled to the given maximum width and height, but:
     *   1- no zoom is applied (i.e. image size is not increased) 
     *   2- aspect ratio is maintained.
     *
     * @param origImg It is the image to use to generate the new scaled image.
     * @param width   Maximun width of the generated image.
     * @param height  Maximun height of the generated image.
     *
     * @return A scaled version of the gicen image
     */
    private static RenderedImage scaledImage(Image origImg, int width, int height) {

	double ratio0 = (double)width / height;
     	
	int w1 = origImg.getWidth(null);
	int h1 = origImg.getHeight(null);
	double ratio1 = (double)w1/h1;

	int w2 = w1;
	int h2 = h1;
	double scale = 1;
	if (ratio1 > ratio0) {
	    if (w1 > width) {
		w2 = width;
		h2 = (int)(width/ratio1);
		scale = (double) width/w1;
	    }
	} else {
	    if (h1 > height) {
		w2 = (int)(height*ratio1);
		h2 = height;
		scale = (double)height/h1;
	    }
	}

	BufferedImage destImg = new BufferedImage(w2, h2, BufferedImage.TYPE_INT_RGB);

     	Graphics2D g = destImg.createGraphics(); 
	// Paint the image onto the buffered image 
	AffineTransform at = AffineTransform.getScaleInstance(scale,scale);
	g.drawImage(origImg, at, null); 
	g.dispose(); 

	return destImg;
    }


    /**
     *  Save the given RenderedImage as a GIF file.
     */
    private static void saveGif(RenderedImage img, String destinyPath) {

	File destinyFile  = new File(destinyPath);
	destinyFile.getAbsoluteFile().getParentFile().mkdirs();

     	try {
	    destinyFile.delete();
	    ImageIO.write(img,"gif", destinyFile);
	} catch (IOException e) {
	    System.err.println("I can not write the \""+destinyPath+"\" image.");
	    e.printStackTrace();
	    System.exit(1);
	}    	
    }


    /**
     *  Save the given RenderedImage as a JPG file.
     */
    private static void saveJpg(RenderedImage img, String destinyPath) {
	
	File destinyFile  = new File(destinyPath);
	destinyFile.getAbsoluteFile().getParentFile().mkdirs();
	
	try {
	    // Get the first jpeg writer
	    ImageWriter writer = ImageIO.getImageWritersByFormatName("jpg").next();
	    
	    // Config output file
	    destinyFile.delete();
	    ImageOutputStream ios = ImageIO.createImageOutputStream(destinyFile);
	    writer.setOutput(ios);
	    
	    // Set the compression quality
	    JPEGImageWriteParam iwparam = new JPEGImageWriteParam(Locale.getDefault());
	    iwparam.setCompressionMode(ImageWriteParam.MODE_EXPLICIT);
	    iwparam.setCompressionQuality(0.9f);

	    // Write the image
	    writer.write(null, new IIOImage(img, null, null), iwparam);
	    
	    // Cleanup
	    ios.flush();
	    writer.dispose();
	    ios.close();
	} catch (IOException e) {
	    System.err.println("I can not write the \""+destinyPath+"\" image.");
	    e.printStackTrace();
	    System.exit(1);
	}
    }
    
    
    public static void main(String args[]) {

	if (args.length != 3) {
	    System.err.println("Usage: java MakeAvatar <orig_filename> <dest_filename> <icon_filename>");
	    System.exit(1);
	}

	String origPath    = args[0];
     	File origFile = new File(origPath);

	Image origImg = null;
	try {
	    origImg = javax.imageio.ImageIO.read(origFile);
	} catch (IOException e) {			
	    System.err.println("I can not read the given image \""+origPath+"\".");
	    e.printStackTrace();
	    System.exit(1);
	}


	RenderedImage destImg = scaledImage(origImg, IMAGE_WIDTH, IMAGE_HEIGHT);
	saveJpg(destImg, args[1]);

	RenderedImage destIcon = scaledImage(origImg, ICON_WIDTH, ICON_HEIGHT);
	saveGif(destIcon, args[2]);
    }
}

