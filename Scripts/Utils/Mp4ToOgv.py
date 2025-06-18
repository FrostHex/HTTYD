import os
from moviepy import VideoFileClip

def convert_mp4_to_ogv(input_file, output_file):
    """
    Convert an MP4 file to OGV format using MoviePy
    Maintains the original framerate while reducing video quality
    """
    try:
        # Load the video
        video = VideoFileClip(input_file)
        
        # Get the original fps
        original_fps = video.fps
        
        # Write the video to output file in ogv format with reduced quality
        # but maintaining the same framerate
        video.write_videofile(
            output_file, 
            codec='libtheora', 
            audio_codec='libvorbis',
            fps=original_fps,           # Maintain original framerate
            bitrate='2000k',            # Reduced bitrate for lower quality
            ffmpeg_params=['-q:v', '10']
        )
        
        # Close the video to release resources
        video.close()
        
        print(f"Successfully converted {input_file} to {output_file}")
        return True
    except Exception as e:
        print(f"Error during conversion: {e}")
        return False

if __name__ == "__main__":
    # Input file in the current directory
    input_file = "Test Drive_1.mp4"
    output_file = "Test Drive.ogv"
    
    # Check if input file exists
    if not os.path.isfile(input_file):
        print(f"Error: Input file '{input_file}' not found in current directory.")
    else:
        convert_mp4_to_ogv(input_file, output_file)
