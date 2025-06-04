import grpc

# Import generated gRPC files
import matrix_service_pb2
import matrix_service_pb2_grpc


class NSMatrix:
    """Represents a matrix managed by the remote execution system."""
    def __init__(self, name: str, rows: int, cols: int):
        """
        Initializes the NSMatrix object.

        Args:
            name: The name of the matrix on the server.
            rows: The number of rows in the matrix.
            cols: The number of columns in the matrix.
        """
        self.name = name
        self.rows = rows
        self.cols = cols

    def __repr__(self):
        return f"NSMatrix(name='{self.name}', rows={self.rows}, cols={self.cols})"

    def get_size(self) -> tuple[int, int]:
        """Returns the size of the matrix."""
        return self.rows, self.cols

    def multiply(self, other: 'NSMatrix', result_name: str, use_transpose: bool = True) -> 'NSMatrix | None':
        """
        Multiply this matrix with another matrix on the server.
        
        Args:
            other: The other matrix to multiply with.
            result_name: The name for the resulting matrix.
            use_transpose: Whether to use the transpose method for multiplication (usually faster).
            
        Returns:
            A new NSMatrix object representing the result of multiplication, or None on failure.
        """
        # We'll implement this in NSProxy and call it from here
        if not hasattr(self, '_proxy'):
            raise RuntimeError("Matrix object not associated with a proxy")
        return self._proxy.multiply_matrices(self, other, result_name, use_transpose)


class NSProxy:
    """
    A Python client to interact with the remote gRPC execution system.
    """
    def __init__(self, server_address: str):
        """
        Initializes the proxy and connects to the gRPC server.

        Args:
            server_address: The address (e.g., 'localhost:50051') of the remote gRPC server.
        """
        # Create a gRPC channel
        self.channel = grpc.insecure_channel(server_address)
        # Create a stub (client)
        self.stub = matrix_service_pb2_grpc.MatrixServiceStub(self.channel)
        print(f"Connected gRPC client to {server_address}")
        self.server_address = server_address  # Keep for potential reference

    def load_matrix_from_file(self, matrix_name: str, file_path: str) -> NSMatrix | None:
        """
        Loads a matrix from a server-side file path onto the remote server via gRPC.

        Args:
            matrix_name: The name to assign to the matrix on the server.
            file_path: The server-side path to the file containing the matrix data.

        Returns:
            An NSMatrix object representing the loaded matrix, or None on failure.
        """
        print(f"Requesting server to load matrix '{matrix_name}' from file '{file_path}'")
        try:
            request = matrix_service_pb2.LoadMatrixRequest(matrix_name=matrix_name, file_path=file_path)
            response = self.stub.LoadMatrixFromFile(request)

            if response.success:
                print(f"Successfully loaded matrix '{matrix_name}'.")
                matrix = NSMatrix(response.matrix_name, response.rows, response.cols)
                matrix._proxy = self
                return matrix
            else:
                print(f"Server failed to load matrix '{matrix_name}': {response.error_message}")
                return None
        except grpc.RpcError as e:
            print(f"gRPC Error loading matrix '{matrix_name}': {e.details()} (Status: {e.code()})")
            return None

    def create_zero_matrix(self, matrix_name: str, n: int, m: int) -> NSMatrix | None:
        """
        Creates a matrix filled with zeros on the remote server via gRPC.

        Args:
            matrix_name: The name to assign to the matrix on the server.
            n: The number of rows.
            m: The number of columns.

        Returns:
            An NSMatrix object representing the created zero matrix, or None on failure.
        """
        print(f"Requesting server to create zero matrix '{matrix_name}' ({n}x{m})")
        try:
            request = matrix_service_pb2.CreateZeroMatrixRequest(matrix_name=matrix_name, rows=n, cols=m)
            response = self.stub.CreateZeroMatrix(request)

            if response.success:
                print(f"Successfully created zero matrix '{matrix_name}'.")
                matrix = NSMatrix(response.matrix_name, response.rows, response.cols)
                matrix._proxy = self
                return matrix
            else:
                print(f"Server failed to create zero matrix '{matrix_name}': {response.error_message}")
                return None
        except grpc.RpcError as e:
            print(f"gRPC Error creating zero matrix '{matrix_name}': {e.details()} (Status: {e.code()})")
            return None

    def get_matrix_size(self, matrix_name: str) -> tuple[int, int] | None:
        """
        Retrieves the size (rows, columns) of a matrix stored on the server via gRPC.

        Args:
            matrix_name: The name of the matrix on the server.

        Returns:
            A tuple (rows, columns) if the matrix exists and retrieval is successful, otherwise None.
        """
        print(f"Requesting size of matrix '{matrix_name}' from server")
        try:
            request = matrix_service_pb2.GetMatrixSizeRequest(matrix_name=matrix_name)
            response = self.stub.GetMatrixSize(request)

            if response.success:
                print(f"Successfully retrieved size for matrix '{matrix_name}'.")
                return (response.rows, response.cols)
            else:
                print(f"Server failed to get size for matrix '{matrix_name}': {response.error_message}")
                return None
        except grpc.RpcError as e:
            print(f"gRPC Error getting size for matrix '{matrix_name}': {e.details()} (Status: {e.code()})")
            return None

    def multiply_matrices(self, matrix_a: NSMatrix, matrix_b: NSMatrix, result_name: str, use_transpose: bool = True) -> NSMatrix | None:
        """
        Multiply two matrices on the server and store the result in a new matrix.

        Args:
            matrix_a: First matrix to multiply.
            matrix_b: Second matrix to multiply.
            result_name: Name for the resulting matrix.
            use_transpose: Whether to use the transpose method for multiplication (usually faster).

        Returns:
            A new NSMatrix object representing the result of multiplication, or None on failure.
        """
        print(f"Requesting multiplication of matrices '{matrix_a.name}' and '{matrix_b.name}' into '{result_name}'")
        try:
            request = matrix_service_pb2.MultiplyMatricesRequest(
                matrix_a_name=matrix_a.name,
                matrix_b_name=matrix_b.name,
                result_name=result_name,
                use_transpose=use_transpose
            )
            response = self.stub.MultiplyMatrices(request)

            if response.success:
                print(f"Successfully multiplied matrices into '{result_name}'.")
                return NSMatrix(response.matrix_name, response.rows, response.cols)
            else:
                print(f"Server failed to multiply matrices: {response.error_message}")
                return None
        except grpc.RpcError as e:
            print(f"gRPC Error multiplying matrices: {e.details()} (Status: {e.code()})")
            return None

    def close(self):
        """Closes the gRPC channel."""
        if self.channel:
            self.channel.close()
            print("gRPC channel closed.")

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


# Example Usage (optional, for testing)
if __name__ == '__main__':
    # Read port from config.json
    import json
    import os

    # Get the directory containing this script
    current_dir = os.path.dirname(os.path.abspath(__file__))
    config_path = os.path.join(current_dir, '..', 'config.json')
    
    try:
        with open(config_path) as f:
            config = json.load(f)
            port = config.get('port', 50051)  # default to 50051 if not found
    except (FileNotFoundError, json.JSONDecodeError) as e:
        print(f"Warning: Could not read config file, using default port 50051: {e}")
        port = 50051

    # Address of the running gRPC server
    server_addr = f"localhost:{port}"

    # Use a context manager to ensure the channel is closed
    with NSProxy(server_addr) as proxy:

        # Example calls
        # Load matrix from our test CSV file
        matrix_obj = proxy.load_matrix_from_file("test_matrix", "test_matrix.csv")
        if matrix_obj:
            print(f"Created matrix object: {matrix_obj}")
            rows, cols = matrix_obj.get_size()
            print(f"Matrix '{matrix_obj.name}' size from object: {rows} rows, {cols} columns")
        else:
            print("Failed to load matrix from file.")

        zero_matrix_obj = proxy.create_zero_matrix("zero_mat_grpc", 5, 10)
        if zero_matrix_obj:
            print(f"Created zero matrix object: {zero_matrix_obj}")
            rows, cols = zero_matrix_obj.get_size()
            print(f"Matrix '{zero_matrix_obj.name}' size from object: {rows} rows, {cols} columns")
        else:
            print("Failed to create zero matrix.")

        # Get size of the zero matrix we just created
        size_info = proxy.get_matrix_size("zero_mat_grpc")
        if size_info:
            print(f"Matrix 'zero_mat_grpc' size from server: {size_info[0]} rows, {size_info[1]} columns")
        else:
            print("Failed to get size for 'zero_mat_grpc'.")

        # Try getting size of a non-existent matrix
        size_info_nonexist = proxy.get_matrix_size("non_existent_matrix")
        if not size_info_nonexist:
            print("Correctly failed to get size for non-existent matrix.")
            
        # Test matrix multiplication if we have our test matrix and zero matrix
        if matrix_obj and zero_matrix_obj:
            # For testing, we'll multiply test_matrix with itself
            result_matrix = matrix_obj.multiply(matrix_obj, "result_matrix", use_transpose=True)
            if result_matrix:
                print(f"Successfully created result matrix: {result_matrix}")
                rows, cols = result_matrix.get_size()
                print(f"Result matrix size: {rows}x{cols}")
            else:
                print("Failed to multiply matrices.")

