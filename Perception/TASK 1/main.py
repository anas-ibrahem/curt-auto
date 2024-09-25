import cv2
import numpy as np

cap = cv2.VideoCapture('Perception/TASK 1/Car.mp4')  # <-- Change the path to the video file

lower_yellow = np.array([20, 100, 100])
upper_yellow = np.array([50, 255, 255])

lower_blue = np.array([90, 86, 90])
upper_blue = np.array([150, 255, 255])

# crop to discard distracting objects and shadows
crop_y_start = 100
crop_y_end = -250

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    cropped_frame = frame[crop_y_start:crop_y_end, :]

    # Convert to HSV color space
    hsv = cv2.cvtColor(cropped_frame, cv2.COLOR_BGR2HSV)

    mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)
    combined_mask = cv2.bitwise_or(mask_yellow, mask_blue)

    # Discard the middle region (For Car itself)
    frame_height, frame_width = cropped_frame.shape[:2]
    exclude_x_start = frame_width // 2 - 200
    exclude_x_end = frame_width // 2 + 200
    exclude_y_start = frame_height // 2
    exclude_y_end = frame_height

    # a mask to exclude the region
    exclusion_mask = np.ones_like(combined_mask, dtype=np.uint8) * 255
    exclusion_mask[exclude_y_start:exclude_y_end, exclude_x_start:exclude_x_end] = 0

    combined_mask = cv2.bitwise_and(combined_mask, exclusion_mask)
    contours, _ = cv2.findContours(combined_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # Loop through contours and filter by size
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 200:
            x, y, w, h = cv2.boundingRect(cnt)
            x += 0
            y += crop_y_start
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

    cv2.imshow('Frame', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()