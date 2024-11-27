# User Guide - Stack and Queue Visualizer

## Getting Started

### Installation
1. Download the latest release
2. Run the installer
3. Launch the application

### First Launch
When you first launch the application, you'll see two main tabs:
- Stack Tab
- Queue Tab

## Using the Stack Visualizer

### Basic Operations

#### Adding Elements (Push)
1. Click on the Stack tab
2. Enter a number in the input field
3. Click the "Push" button
4. Watch as the element is added to the top of the stack

#### Removing Elements (Pop)
1. Click the "Pop" button
2. The topmost element will be removed
3. A message will show the removed value

#### Viewing Top Element (Peek)
1. Click the "Peek" button
2. A message will show the current top value
3. The element remains in the stack

#### Clearing the Stack
1. Click the "Clear" button
2. All elements will be removed
3. The stack returns to empty state

### Understanding the Visualization
- Elements are shown vertically
- Newest elements appear at the top
- Different colors indicate:
  - Green: Recently added elements
  - Red: Elements about to be removed
  - Blue: Current elements

## Using the Queue Visualizer

### Basic Operations

#### Adding Elements (Enqueue)
1. Click on the Queue tab
2. Enter a number in the input field
3. Click the "Enqueue" button
4. Watch as the element is added to the rear

#### Removing Elements (Dequeue)
1. Click the "Dequeue" button
2. The front element will be removed
3. A message will show the removed value

#### Viewing Elements
1. "Front" button shows the first element
2. "Rear" button shows the last element
3. Elements remain in the queue

#### Clearing the Queue
1. Click the "Clear" button
2. All elements will be removed
3. The queue returns to empty state

### Understanding the Visualization
- Elements are shown horizontally
- Front elements are on the left
- Rear elements are on the right
- Different colors indicate:
  - Blue: Recently added elements
  - Orange: Elements about to be removed
  - Green: Current elements

## Additional Features

### Customizing the View
1. Click "Settings" in the menu bar
2. Adjust the array size (max 50)
3. Choose your preferred theme
4. Save your settings

### Using Demo Mode
1. Click "Demo" in the menu bar
2. Choose Stack or Queue demo
3. Watch the step-by-step demonstration
4. Click "Stop" to end the demo

### Saving and Loading States
1. To Save:
   - Click File → Save State
   - Choose a location
   - Name your file
   - Click Save

2. To Load:
   - Click File → Load State
   - Select a saved file
   - Click Open

## Tips and Tricks

### For Stack Operations
- Watch the top pointer movement
- Notice how LIFO (Last In, First Out) works
- Use Peek before Pop to avoid errors

### For Queue Operations
- Watch both front and rear pointers
- Notice how FIFO (First In, First Out) works
- Check front before dequeue

### General Tips
- Use the status messages for feedback
- Try the demo mode to learn concepts
- Save states to resume later
- Use tooltips for quick help

## Troubleshooting

### Common Issues

#### Stack/Queue is Full
- Check the current size setting
- Remove some elements
- Consider increasing max size in settings

#### Cannot Add Elements
- Verify input is a valid number
- Check if structure is full
- Try clearing and starting fresh

#### Visualization Not Updating
- Close and reopen the application
- Check if animations are enabled
- Verify system resources

### Getting Help
- Click Help → About for basic info
- Visit our website for tutorials
- Contact support for assistance

## Keyboard Shortcuts

### Global Shortcuts
- Ctrl+S: Save State
- Ctrl+L: Load State
- Ctrl+Q: Quit
- F1: Help

### Stack Operations
- Ctrl+P: Push
- Ctrl+O: Pop
- Ctrl+K: Peek
- Ctrl+C: Clear

### Queue Operations
- Ctrl+E: Enqueue
- Ctrl+D: Dequeue
- Ctrl+F: View Front
- Ctrl+R: View Rear
- Ctrl+C: Clear
